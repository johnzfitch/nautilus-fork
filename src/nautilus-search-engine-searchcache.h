/*
 * Nautilus search-cache Search Provider
 *
 * This file is part of search-and-filter integration for Omarchy Linux.
 * Provides lightning-fast file search using search-cache backend.
 *
 * License: MIT
 * Author: Zack <zack@omarchy.dev>
 */

#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define NAUTILUS_TYPE_SEARCH_ENGINE_SEARCHCACHE (nautilus_search_engine_searchcache_get_type ())

G_DECLARE_FINAL_TYPE (NautilusSearchEngineSearchCache, nautilus_search_engine_searchcache,
                      NAUTILUS, SEARCH_ENGINE_SEARCHCACHE, GObject)

NautilusSearchEngineSearchCache *nautilus_search_engine_searchcache_new (void);

G_END_DECLS
