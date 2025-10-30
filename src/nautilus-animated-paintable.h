/*
 * nautilus-animated-paintable.h: Animated paintable for GTK4
 *
 * Copyright (C) 2025 Zack Freedman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 */

#pragma once

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

#define NAUTILUS_TYPE_ANIMATED_PAINTABLE (nautilus_animated_paintable_get_type())

G_DECLARE_FINAL_TYPE (NautilusAnimatedPaintable, nautilus_animated_paintable, NAUTILUS, ANIMATED_PAINTABLE, GObject)

NautilusAnimatedPaintable *nautilus_animated_paintable_new (GdkPixbufAnimation *animation);

void nautilus_animated_paintable_start (NautilusAnimatedPaintable *self);
void nautilus_animated_paintable_stop (NautilusAnimatedPaintable *self);
gboolean nautilus_animated_paintable_is_playing (NautilusAnimatedPaintable *self);

G_END_DECLS
