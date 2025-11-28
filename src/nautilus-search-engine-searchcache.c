/*
 * Nautilus search-cache Search Provider
 *
 * This file is part of search-and-filter integration for Omarchy Linux.
 * Provides lightning-fast file search using search-cache backend.
 *
 * License: MIT
 * Author: Zack <zack@omarchy.dev>
 */

#define G_LOG_DOMAIN "nautilus-search"

#include <config.h>
#include "nautilus-search-engine-searchcache.h"

#include "nautilus-file.h"
#include "nautilus-query.h"
#include "nautilus-search-hit.h"
#include "nautilus-search-provider.h"

#include <string.h>
#include <gio/gio.h>

struct _NautilusSearchEngineSearchCache
{
    GObject parent_instance;

    NautilusQuery *query;
    GCancellable *cancellable;

    gboolean query_pending;
    GQueue *hits_pending;
};

static void nautilus_search_provider_init (NautilusSearchProviderInterface *iface);

G_DEFINE_TYPE_WITH_CODE (NautilusSearchEngineSearchCache,
                         nautilus_search_engine_searchcache,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (NAUTILUS_TYPE_SEARCH_PROVIDER,
                                                nautilus_search_provider_init))

#define BATCH_SIZE 100
#define MAX_RESULTS 500

static void
finalize (GObject *object)
{
    NautilusSearchEngineSearchCache *self = NAUTILUS_SEARCH_ENGINE_SEARCHCACHE (object);

    if (self->cancellable)
    {
        g_cancellable_cancel (self->cancellable);
        g_clear_object (&self->cancellable);
    }

    g_clear_object (&self->query);
    g_queue_free_full (self->hits_pending, g_object_unref);

    G_OBJECT_CLASS (nautilus_search_engine_searchcache_parent_class)->finalize (object);
}

static void
check_pending_hits (NautilusSearchEngineSearchCache *self,
                    gboolean                         force_send)
{
    if (!force_send &&
        g_queue_get_length (self->hits_pending) < BATCH_SIZE)
    {
        return;
    }

    NautilusSearchHit *hit;
    g_autoptr (GPtrArray) hits = g_ptr_array_new_with_free_func (g_object_unref);

    g_debug ("SearchCache engine add hits");

    while ((hit = g_queue_pop_head (self->hits_pending)))
    {
        g_ptr_array_add (hits, hit);
    }

    if (hits->len > 0)
    {
        nautilus_search_provider_hits_added (NAUTILUS_SEARCH_PROVIDER (self),
                                             g_steal_pointer (&hits));
    }
}

static void
search_finished (NautilusSearchEngineSearchCache *self,
                 GError                          *error)
{
    g_debug ("SearchCache engine finished");

    if (error == NULL)
    {
        check_pending_hits (self, TRUE);
    }
    else
    {
        g_queue_foreach (self->hits_pending, (GFunc) g_object_unref, NULL);
        g_queue_clear (self->hits_pending);
    }

    self->query_pending = FALSE;

    if (error && !g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
    {
        g_debug ("SearchCache engine error %s", error->message);
        nautilus_search_provider_error (NAUTILUS_SEARCH_PROVIDER (self), error->message);
    }
    else
    {
        nautilus_search_provider_finished (NAUTILUS_SEARCH_PROVIDER (self),
                                           NAUTILUS_SEARCH_PROVIDER_STATUS_NORMAL);
        if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        {
            g_debug ("SearchCache engine cancelled");
        }
    }
}

static void
process_search_results (NautilusSearchEngineSearchCache *self,
                       const gchar                     *output)
{
    g_auto (GStrv) lines = g_strsplit (output, "\n", -1);

    for (gsize i = 0; lines[i] != NULL; i++)
    {
        const gchar *line = g_strstrip (lines[i]);

        if (line[0] == '\0')
        {
            continue;
        }

        // Create GFile from path and get URI
        g_autoptr (GFile) file = g_file_new_for_path (line);
        g_autofree gchar *uri = g_file_get_uri (file);

        // Create search hit with URI
        NautilusSearchHit *hit = nautilus_search_hit_new (uri);

        // Set relevance (search-cache doesn't provide scoring, use fixed value)
        nautilus_search_hit_set_fts_rank (hit, 0.5);

        // Add to pending hits
        g_queue_push_tail (self->hits_pending, hit);

        // Batch send hits
        check_pending_hits (self, FALSE);
    }
}

static void
search_callback (GObject      *source_object,
                GAsyncResult *result,
                gpointer      user_data)
{
    NautilusSearchEngineSearchCache *self = user_data;
    g_autoptr (GError) error = NULL;

    g_autofree gchar *stdout_str = NULL;
    g_autofree gchar *stderr_str = NULL;
    gint exit_status;

    GSubprocess *subprocess = G_SUBPROCESS (source_object);

    if (!g_subprocess_communicate_utf8_finish (subprocess,
                                               result,
                                               &stdout_str,
                                               &stderr_str,
                                               &error))
    {
        // FALLBACK: On communication error, silently finish (let other engines work)
        g_debug ("SearchCache engine: communication failed, falling back silently");
        search_finished (self, NULL);
        return;
    }

    exit_status = g_subprocess_get_exit_status (subprocess);

    if (exit_status != 0)
    {
        // FALLBACK: On sc error (missing index, bad query), silently finish
        // Don't report error to user - other search engines (simple, tracker) will provide results
        g_debug ("SearchCache engine: sc exited with status %d, falling back: %s",
                 exit_status, stderr_str ? stderr_str : "");
        search_finished (self, NULL);
        return;
    }

    if (stdout_str && stdout_str[0] != '\0')
    {
        process_search_results (self, stdout_str);
    }

    search_finished (self, NULL);
}

static gboolean
search_engine_searchcache_start (NautilusSearchProvider *provider,
                                 NautilusQuery          *query)
{
    NautilusSearchEngineSearchCache *self = NAUTILUS_SEARCH_ENGINE_SEARCHCACHE (provider);

    g_debug ("SearchCache engine start");

    if (self->query_pending)
    {
        g_debug ("SearchCache engine already running, cancelling");
        g_cancellable_cancel (self->cancellable);
        g_clear_object (&self->cancellable);
    }

    g_clear_object (&self->query);
    self->query = g_object_ref (query);
    self->cancellable = g_cancellable_new ();

    const gchar *search_text = nautilus_query_get_text (query);

    if (search_text == NULL || search_text[0] == '\0')
    {
        g_debug ("SearchCache engine: empty query");
        return FALSE;
    }

    g_debug ("SearchCache engine: searching for '%s'", search_text);

    // RESILIENCE: Check if sc binary exists in PATH
    // If not, silently return and let other search engines handle it
    g_autofree gchar *sc_path = g_find_program_in_path ("sc");
    if (sc_path == NULL)
    {
        g_debug ("SearchCache engine: 'sc' not found in PATH, skipping");
        nautilus_search_provider_finished (NAUTILUS_SEARCH_PROVIDER (self),
                                           NAUTILUS_SEARCH_PROVIDER_STATUS_NORMAL);
        return FALSE;
    }

    // Build command: sc --full-path <query>
    g_autoptr (GError) error = NULL;
    g_autoptr (GSubprocessLauncher) launcher = g_subprocess_launcher_new (
        G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_PIPE);

    // Get location filter if any
    GFile *location = nautilus_query_get_location (query);
    const gchar *location_path = NULL;

    if (location != NULL)
    {
        location_path = g_file_get_path (location);
    }

    // Launch subprocess with result limit to prevent CPU overload
    g_autofree gchar *limit_str = g_strdup_printf ("%d", MAX_RESULTS);
    GSubprocess *subprocess;

    // Note: sc doesn't have --path filtering yet, so we do global search
    // and let the query handle path-based filtering (e.g., "/dev/" queries)
    (void)location_path;  // Suppress unused warning

    subprocess = g_subprocess_launcher_spawn (launcher, &error,
                                              "sc",
                                              "--full-path",
                                              "--limit", limit_str,
                                              search_text,
                                              NULL);

    if (subprocess == NULL)
    {
        g_warning ("Failed to spawn search-cache: %s", error->message);
        search_finished (self, error);
        return FALSE;
    }

    self->query_pending = TRUE;

    // Communicate asynchronously
    g_subprocess_communicate_utf8_async (subprocess,
                                         NULL,
                                         self->cancellable,
                                         search_callback,
                                         self);

    g_object_unref (subprocess);

    return TRUE;
}

static void
nautilus_search_engine_searchcache_stop (NautilusSearchProvider *provider)
{
    NautilusSearchEngineSearchCache *self = NAUTILUS_SEARCH_ENGINE_SEARCHCACHE (provider);

    g_debug ("SearchCache engine stop");

    if (self->query_pending)
    {
        g_cancellable_cancel (self->cancellable);
        g_clear_object (&self->cancellable);
        self->query_pending = FALSE;
    }
}

static void
nautilus_search_provider_init (NautilusSearchProviderInterface *iface)
{
    iface->start = search_engine_searchcache_start;
    iface->stop = nautilus_search_engine_searchcache_stop;
}

static void
nautilus_search_engine_searchcache_class_init (NautilusSearchEngineSearchCacheClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);

    object_class->finalize = finalize;
}

static void
nautilus_search_engine_searchcache_init (NautilusSearchEngineSearchCache *self)
{
    self->hits_pending = g_queue_new ();

    g_debug ("SearchCache provider initialized");
}

NautilusSearchEngineSearchCache *
nautilus_search_engine_searchcache_new (void)
{
    return g_object_new (NAUTILUS_TYPE_SEARCH_ENGINE_SEARCHCACHE, NULL);
}
