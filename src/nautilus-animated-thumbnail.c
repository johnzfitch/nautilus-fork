/*
 * nautilus-animated-thumbnail.c: Animated thumbnail support
 *
 * Copyright (C) 2025 Zack Freedman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#define G_LOG_DOMAIN "nautilus-animated-thumbnail"

#include "nautilus-animated-thumbnail.h"
#include "nautilus-global-preferences.h"
#include <gtk/gtk.h>
#include <string.h>

/* Animation cache to avoid loading same animations multiple times */
static GHashTable *animation_cache = NULL;

/* Maximum number of cached animations (memory management) */
#define MAX_CACHED_ANIMATIONS 50

/* Supported animated formats */
static const char *animated_mime_types[] = {
    "image/webp",
    "image/gif",
    "image/apng",
    "image/png",  /* PNG can be animated (APNG) */
    NULL
};

struct _NautilusAnimationIterator {
    GdkPixbufAnimation *animation;
    GdkPixbufAnimationIter *iter;
    GTimeVal time;
};

void
nautilus_animated_thumbnail_init (void)
{
    if (animation_cache == NULL)
    {
        animation_cache = g_hash_table_new_full (g_str_hash,
                                                   g_str_equal,
                                                   g_free,
                                                   g_object_unref);
    }
}

void
nautilus_animated_thumbnail_shutdown (void)
{
    if (animation_cache != NULL)
    {
        g_hash_table_destroy (animation_cache);
        animation_cache = NULL;
    }
}

gboolean
nautilus_animated_thumbnail_is_supported (const char *mime_type)
{
    int i;

    if (mime_type == NULL)
    {
        return FALSE;
    }

    for (i = 0; animated_mime_types[i] != NULL; i++)
    {
        if (g_strcmp0 (mime_type, animated_mime_types[i]) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

gboolean
nautilus_animated_thumbnail_is_animated (const char *file_path)
{
    g_autoptr (GdkPixbufAnimation) animation = NULL;
    g_autoptr (GError) error = NULL;

    if (file_path == NULL)
    {
        return FALSE;
    }

    animation = gdk_pixbuf_animation_new_from_file (file_path, &error);

    if (error != NULL)
    {
        g_debug ("Failed to check if file is animated: %s", error->message);
        return FALSE;
    }

    if (animation == NULL)
    {
        return FALSE;
    }

    /* Check if animation has multiple frames */
    return !gdk_pixbuf_animation_is_static_image (animation);
}

GdkPixbufAnimation *
nautilus_animated_thumbnail_load (const char *file_path,
                                   GError **error)
{
    GdkPixbufAnimation *animation;

    g_return_val_if_fail (file_path != NULL, NULL);

    animation = gdk_pixbuf_animation_new_from_file (file_path, error);

    if (*error != NULL)
    {
        g_warning ("Failed to load animated thumbnail: %s", (*error)->message);
        return NULL;
    }

    return animation;
}

NautilusAnimationMode
nautilus_animated_thumbnail_get_mode (void)
{
    g_autofree char *mode_str = NULL;

    mode_str = g_settings_get_string (nautilus_preferences,
                                       NAUTILUS_PREFERENCES_ANIMATED_THUMBNAILS);

    if (g_strcmp0 (mode_str, "never") == 0)
    {
        return NAUTILUS_ANIMATION_MODE_NEVER;
    }
    else if (g_strcmp0 (mode_str, "on-hover") == 0)
    {
        return NAUTILUS_ANIMATION_MODE_ON_HOVER;
    }
    else if (g_strcmp0 (mode_str, "on-select") == 0)
    {
        return NAUTILUS_ANIMATION_MODE_ON_SELECT;
    }
    else if (g_strcmp0 (mode_str, "always") == 0)
    {
        return NAUTILUS_ANIMATION_MODE_ALWAYS;
    }

    /* Default to on-select */
    return NAUTILUS_ANIMATION_MODE_ON_SELECT;
}

void
nautilus_animated_thumbnail_cache_add (const char *uri,
                                        GdkPixbufAnimation *animation)
{
    g_return_if_fail (uri != NULL);
    g_return_if_fail (GDK_IS_PIXBUF_ANIMATION (animation));

    if (animation_cache == NULL)
    {
        nautilus_animated_thumbnail_init ();
    }

    /* Check cache size and evict if necessary */
    if (g_hash_table_size (animation_cache) >= MAX_CACHED_ANIMATIONS)
    {
        /* Simple LRU: just clear cache when full */
        g_hash_table_remove_all (animation_cache);
        g_debug ("Animation cache full, cleared all entries");
    }

    g_hash_table_insert (animation_cache,
                          g_strdup (uri),
                          g_object_ref (animation));
}

GdkPixbufAnimation *
nautilus_animated_thumbnail_cache_get (const char *uri)
{
    GdkPixbufAnimation *animation;

    g_return_val_if_fail (uri != NULL, NULL);

    if (animation_cache == NULL)
    {
        return NULL;
    }

    animation = g_hash_table_lookup (animation_cache, uri);

    if (animation != NULL)
    {
        g_object_ref (animation);
    }

    return animation;
}

void
nautilus_animated_thumbnail_cache_remove (const char *uri)
{
    g_return_if_fail (uri != NULL);

    if (animation_cache != NULL)
    {
        g_hash_table_remove (animation_cache, uri);
    }
}

void
nautilus_animated_thumbnail_cache_clear (void)
{
    if (animation_cache != NULL)
    {
        g_hash_table_remove_all (animation_cache);
    }
}

/* Animation iterator implementation */

NautilusAnimationIterator *
nautilus_animation_iterator_new (GdkPixbufAnimation *animation)
{
    NautilusAnimationIterator *iter;

    g_return_val_if_fail (GDK_IS_PIXBUF_ANIMATION (animation), NULL);

    iter = g_new0 (NautilusAnimationIterator, 1);
    iter->animation = g_object_ref (animation);

    g_get_current_time (&iter->time);
    iter->iter = gdk_pixbuf_animation_get_iter (animation, &iter->time);

    return iter;
}

void
nautilus_animation_iterator_free (NautilusAnimationIterator *iter)
{
    if (iter == NULL)
    {
        return;
    }

    if (iter->iter != NULL)
    {
        g_object_unref (iter->iter);
    }

    if (iter->animation != NULL)
    {
        g_object_unref (iter->animation);
    }

    g_free (iter);
}

GdkPixbuf *
nautilus_animation_iterator_get_pixbuf (NautilusAnimationIterator *iter)
{
    g_return_val_if_fail (iter != NULL, NULL);
    g_return_val_if_fail (iter->iter != NULL, NULL);

    return gdk_pixbuf_animation_iter_get_pixbuf (iter->iter);
}

gboolean
nautilus_animation_iterator_advance (NautilusAnimationIterator *iter)
{
    g_return_val_if_fail (iter != NULL, FALSE);
    g_return_val_if_fail (iter->iter != NULL, FALSE);

    g_get_current_time (&iter->time);
    return gdk_pixbuf_animation_iter_advance (iter->iter, &iter->time);
}

int
nautilus_animation_iterator_get_delay_time (NautilusAnimationIterator *iter)
{
    g_return_val_if_fail (iter != NULL, -1);
    g_return_val_if_fail (iter->iter != NULL, -1);

    return gdk_pixbuf_animation_iter_get_delay_time (iter->iter);
}
