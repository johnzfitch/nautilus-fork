/*
 * nautilus-animated-thumbnail.h: Animated thumbnail support
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

#pragma once

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib-object.h>

G_BEGIN_DECLS

/* Animation playback modes */
typedef enum {
    NAUTILUS_ANIMATION_MODE_NEVER,
    NAUTILUS_ANIMATION_MODE_ON_HOVER,
    NAUTILUS_ANIMATION_MODE_ON_SELECT,
    NAUTILUS_ANIMATION_MODE_ALWAYS
} NautilusAnimationMode;

/* Check if a file supports animation */
gboolean nautilus_animated_thumbnail_is_supported (const char *mime_type);

/* Check if a file is actually animated (not just a static image in animated format) */
gboolean nautilus_animated_thumbnail_is_animated (const char *file_path);

/* Load animated thumbnail from file */
GdkPixbufAnimation *nautilus_animated_thumbnail_load (const char *file_path,
                                                       GError **error);

/* Get current animation mode from settings */
NautilusAnimationMode nautilus_animated_thumbnail_get_mode (void);

/* Animation cache management */
void nautilus_animated_thumbnail_cache_add (const char *uri,
                                             GdkPixbufAnimation *animation);
GdkPixbufAnimation *nautilus_animated_thumbnail_cache_get (const char *uri);
void nautilus_animated_thumbnail_cache_remove (const char *uri);
void nautilus_animated_thumbnail_cache_clear (void);

/* Animation playback control */
typedef struct _NautilusAnimationIterator NautilusAnimationIterator;

NautilusAnimationIterator *nautilus_animation_iterator_new (GdkPixbufAnimation *animation);
void nautilus_animation_iterator_free (NautilusAnimationIterator *iter);
GdkPixbuf *nautilus_animation_iterator_get_pixbuf (NautilusAnimationIterator *iter);
gboolean nautilus_animation_iterator_advance (NautilusAnimationIterator *iter);
int nautilus_animation_iterator_get_delay_time (NautilusAnimationIterator *iter);

/* Initialize/cleanup */
void nautilus_animated_thumbnail_init (void);
void nautilus_animated_thumbnail_shutdown (void);

G_END_DECLS
