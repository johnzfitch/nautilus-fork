/*
 * nautilus-animated-paintable.c: Animated paintable for GTK4
 *
 * Copyright (C) 2025 Zack Freedman
 */

#define G_LOG_DOMAIN "nautilus-animated-paintable"

#include "nautilus-animated-paintable.h"

struct _NautilusAnimatedPaintable
{
    GObject parent_instance;

    GdkPixbufAnimation *animation;
    GdkPixbufAnimationIter *iter;
    GdkTexture *current_texture;
    guint timeout_id;
    gboolean is_playing;
};

static void nautilus_animated_paintable_paintable_init (GdkPaintableInterface *iface);

G_DEFINE_TYPE_WITH_CODE (NautilusAnimatedPaintable, nautilus_animated_paintable, G_TYPE_OBJECT,
                          G_IMPLEMENT_INTERFACE (GDK_TYPE_PAINTABLE,
                                                 nautilus_animated_paintable_paintable_init))

static void
update_texture (NautilusAnimatedPaintable *self)
{
    GdkPixbuf *pixbuf;

    if (self->iter == NULL)
    {
        return;
    }

    pixbuf = gdk_pixbuf_animation_iter_get_pixbuf (self->iter);

    if (pixbuf != NULL)
    {
        g_clear_object (&self->current_texture);
        self->current_texture = gdk_texture_new_for_pixbuf (pixbuf);
        gdk_paintable_invalidate_contents (GDK_PAINTABLE (self));
    }
}

static gboolean
advance_frame (gpointer data)
{
    NautilusAnimatedPaintable *self = NAUTILUS_ANIMATED_PAINTABLE (data);
    GTimeVal current_time;
    int delay;

    if (!self->is_playing || self->iter == NULL)
    {
        self->timeout_id = 0;
        return G_SOURCE_REMOVE;
    }

    g_get_current_time (&current_time);
    gdk_pixbuf_animation_iter_advance (self->iter, &current_time);

    update_texture (self);

    delay = gdk_pixbuf_animation_iter_get_delay_time (self->iter);
    if (delay >= 0)
    {
        self->timeout_id = g_timeout_add (delay, advance_frame, self);
    }

    return G_SOURCE_REMOVE;
}

static void
nautilus_animated_paintable_snapshot (GdkPaintable *paintable,
                                       GdkSnapshot  *snapshot,
                                       double        width,
                                       double        height)
{
    NautilusAnimatedPaintable *self = NAUTILUS_ANIMATED_PAINTABLE (paintable);

    if (self->current_texture != NULL)
    {
        gdk_paintable_snapshot (GDK_PAINTABLE (self->current_texture),
                                 snapshot, width, height);
    }
}

static int
nautilus_animated_paintable_get_intrinsic_width (GdkPaintable *paintable)
{
    NautilusAnimatedPaintable *self = NAUTILUS_ANIMATED_PAINTABLE (paintable);

    if (self->animation != NULL)
    {
        return gdk_pixbuf_animation_get_width (self->animation);
    }

    return 0;
}

static int
nautilus_animated_paintable_get_intrinsic_height (GdkPaintable *paintable)
{
    NautilusAnimatedPaintable *self = NAUTILUS_ANIMATED_PAINTABLE (paintable);

    if (self->animation != NULL)
    {
        return gdk_pixbuf_animation_get_height (self->animation);
    }

    return 0;
}

static void
nautilus_animated_paintable_paintable_init (GdkPaintableInterface *iface)
{
    iface->snapshot = nautilus_animated_paintable_snapshot;
    iface->get_intrinsic_width = nautilus_animated_paintable_get_intrinsic_width;
    iface->get_intrinsic_height = nautilus_animated_paintable_get_intrinsic_height;
}

static void
nautilus_animated_paintable_finalize (GObject *object)
{
    NautilusAnimatedPaintable *self = NAUTILUS_ANIMATED_PAINTABLE (object);

    nautilus_animated_paintable_stop (self);

    g_clear_object (&self->animation);
    g_clear_object (&self->iter);
    g_clear_object (&self->current_texture);

    G_OBJECT_CLASS (nautilus_animated_paintable_parent_class)->finalize (object);
}

static void
nautilus_animated_paintable_class_init (NautilusAnimatedPaintableClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = nautilus_animated_paintable_finalize;
}

static void
nautilus_animated_paintable_init (NautilusAnimatedPaintable *self)
{
    self->is_playing = FALSE;
    self->timeout_id = 0;
}

NautilusAnimatedPaintable *
nautilus_animated_paintable_new (GdkPixbufAnimation *animation)
{
    NautilusAnimatedPaintable *self;
    GTimeVal time;

    g_return_val_if_fail (GDK_IS_PIXBUF_ANIMATION (animation), NULL);

    self = g_object_new (NAUTILUS_TYPE_ANIMATED_PAINTABLE, NULL);
    self->animation = g_object_ref (animation);

    g_get_current_time (&time);
    self->iter = gdk_pixbuf_animation_get_iter (animation, &time);

    update_texture (self);

    return self;
}

void
nautilus_animated_paintable_start (NautilusAnimatedPaintable *self)
{
    int delay;

    g_return_if_fail (NAUTILUS_IS_ANIMATED_PAINTABLE (self));

    if (self->is_playing)
    {
        return;
    }

    self->is_playing = TRUE;

    if (self->iter != NULL)
    {
        delay = gdk_pixbuf_animation_iter_get_delay_time (self->iter);
        if (delay >= 0)
        {
            self->timeout_id = g_timeout_add (delay, advance_frame, self);
        }
    }
}

void
nautilus_animated_paintable_stop (NautilusAnimatedPaintable *self)
{
    g_return_if_fail (NAUTILUS_IS_ANIMATED_PAINTABLE (self));

    self->is_playing = FALSE;

    if (self->timeout_id != 0)
    {
        g_source_remove (self->timeout_id);
        self->timeout_id = 0;
    }
}

gboolean
nautilus_animated_paintable_is_playing (NautilusAnimatedPaintable *self)
{
    g_return_val_if_fail (NAUTILUS_IS_ANIMATED_PAINTABLE (self), FALSE);

    return self->is_playing;
}
