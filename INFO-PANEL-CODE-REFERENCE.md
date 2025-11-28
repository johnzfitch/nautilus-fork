# Nautilus Info Panel - Code Reference & Implementation Snippets

## 1. New Files to Create

### src/nautilus-info-panel.h
```c
/*
 * nautilus-info-panel.h: Info panel showing file details and preview
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
#include "nautilus-file.h"

G_BEGIN_DECLS

#define NAUTILUS_TYPE_INFO_PANEL (nautilus_info_panel_get_type())

G_DECLARE_FINAL_TYPE (NautilusInfoPanel, nautilus_info_panel,
                      NAUTILUS, INFO_PANEL, GtkBox)

NautilusInfoPanel *nautilus_info_panel_new (void);

void nautilus_info_panel_set_file (NautilusInfoPanel *self,
                                    NautilusFile *file);

void nautilus_info_panel_clear (NautilusInfoPanel *self);

void nautilus_info_panel_show_animation_preference_changed (NautilusInfoPanel *self);

G_END_DECLS
```

### src/nautilus-info-panel.c Structure
```c
/*
 * nautilus-info-panel.c: Implementation
 */

#include "nautilus-info-panel.h"
#include "nautilus-file.h"
#include "nautilus-animated-thumbnail.h"
#include "nautilus-animated-paintable.h"
#include <glib/gi18n.h>

struct _NautilusInfoPanel {
    GtkBox parent_instance;

    /* Main layout */
    GtkScrolledWindow *scrolled_window;
    GtkBox *main_box;

    /* Preview section */
    GtkFrame *preview_frame;
    GtkBox *preview_box;
    GtkImage *preview_image;
    GtkPicture *animated_picture;
    NautilusAnimatedPaintable *animation_paintable;

    /* Metadata sections */
    GtkBox *metadata_box;
    GtkExpander *general_expander;
    GtkExpander *permissions_expander;

    /* Current file */
    NautilusFile *file;
    GCancellable *cancellable;
};

G_DEFINE_TYPE (NautilusInfoPanel, nautilus_info_panel, GTK_TYPE_BOX)

/* Initialize widget */
static void
nautilus_info_panel_init (NautilusInfoPanel *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));

    /* Setup scrolled window */
    self->scrolled_window = gtk_scrolled_window_new ();
    gtk_scrolled_window_set_policy (
        GTK_SCROLLED_WINDOW (self->scrolled_window),
        GTK_POLICY_NEVER,
        GTK_POLICY_AUTOMATIC
    );

    /* Setup main box */
    self->main_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
    gtk_box_set_homogeneous (GTK_BOX (self->main_box), FALSE);
    gtk_scrolled_window_set_child (
        GTK_SCROLLED_WINDOW (self->scrolled_window),
        GTK_WIDGET (self->main_box)
    );

    /* Setup preview section */
    create_preview_section (self);

    /* Setup metadata section */
    create_metadata_section (self);

    /* Add scrolled window to panel */
    gtk_box_append (GTK_BOX (self), GTK_WIDGET (self->scrolled_window));
}

static void
nautilus_info_panel_class_init (NautilusInfoPanelClass *klass)
{
}

NautilusInfoPanel *
nautilus_info_panel_new (void)
{
    return g_object_new (NAUTILUS_TYPE_INFO_PANEL, NULL);
}

static void
load_preview_for_file (NautilusInfoPanel *self,
                       NautilusFile *file)
{
    if (file == NULL) {
        gtk_widget_set_visible (
            GTK_WIDGET (self->preview_frame),
            FALSE
        );
        return;
    }

    const char *mime_type = nautilus_file_get_mime_type (file);
    char *path = nautilus_file_get_path (file);

    /* Check if file supports animation */
    if (nautilus_animated_thumbnail_is_supported (mime_type)) {
        GdkPixbufAnimation *animation =
            nautilus_animated_thumbnail_load (path, NULL);

        if (animation != NULL) {
            NautilusAnimatedPaintable *paintable =
                nautilus_animated_paintable_new (animation);

            gtk_picture_set_paintable (
                GTK_PICTURE (self->animated_picture),
                GDK_PAINTABLE (paintable)
            );

            /* Start based on preference */
            NautilusAnimationMode mode =
                nautilus_animated_thumbnail_get_mode ();

            if (mode != NAUTILUS_ANIMATION_MODE_NEVER) {
                nautilus_animated_paintable_start (paintable);
            }

            self->animation_paintable = paintable;
            g_object_unref (animation);
            goto done;
        }
    }

    /* Try static image */
    load_static_preview (self, path, mime_type);

done:
    g_free (path);
}

static void
load_metadata_for_file (NautilusInfoPanel *self,
                        NautilusFile *file)
{
    if (file == NULL) {
        return;
    }

    /* Clear previous metadata */
    clear_metadata_section (self);

    /* Add general metadata */
    add_metadata_row (self, _("Name"),
                      nautilus_file_get_display_name (file));

    add_metadata_row (self, _("Type"),
                      nautilus_file_get_mime_type (file));

    char *size_str = g_format_size (nautilus_file_get_size (file));
    add_metadata_row (self, _("Size"), size_str);
    g_free (size_str);

    /* Add date information */
    GDateTime *mod_time = nautilus_file_get_modification_date (file);
    if (mod_time != NULL) {
        char *date_str = g_date_time_format (mod_time, "%F %T");
        add_metadata_row (self, _("Modified"), date_str);
        g_free (date_str);
    }

    /* Add permissions if it's a local file */
    if (nautilus_file_is_local (file)) {
        guint mode = nautilus_file_get_permissions (file);
        char perms[10];
        format_permissions (mode, perms, sizeof (perms));
        add_metadata_row (self, _("Permissions"), perms);
    }
}

void
nautilus_info_panel_set_file (NautilusInfoPanel *self,
                               NautilusFile *file)
{
    g_return_if_fail (NAUTILUS_IS_INFO_PANEL (self));

    if (self->file != NULL) {
        g_object_unref (self->file);
    }

    if (file != NULL) {
        self->file = g_object_ref (file);
        load_preview_for_file (self, file);
        load_metadata_for_file (self, file);
        gtk_widget_set_visible (GTK_WIDGET (self), TRUE);
    } else {
        self->file = NULL;
        gtk_widget_set_visible (GTK_WIDGET (self), FALSE);
    }
}

void
nautilus_info_panel_clear (NautilusInfoPanel *self)
{
    g_return_if_fail (NAUTILUS_IS_INFO_PANEL (self));
    nautilus_info_panel_set_file (self, NULL);
}
```

## 2. Files to Modify

### src/nautilus-window.ui

**Find:**
```xml
<property name="content">
  <object class="AdwToolbarView">
    <child type="top">
      <object class="NautilusToolbar" id="toolbar">
        <property name="show-view-controls">true</property>
        <property name="show-history-controls">true</property>
        <property name="sidebar-button-active" bind-source="split_view" bind-property="show-sidebar" bind-flags="bidirectional|sync-create"/>
        <property name="window-slot" bind-source="NautilusWindow" bind-property="active-slot" bind-flags="sync-create"/>
      </object>
    </child>
    <child type="top">
      <object class="AdwTabBar" id="tab_bar">
        <property name="view">tab_view</property>
      </object>
    </child>
    <property name="content">
      <object class="AdwTabView" id="tab_view">
        <property name="menu-model">tab_menu_model</property>
      </object>
    </property>
```

**Replace with:**
```xml
<property name="content">
  <object class="AdwToolbarView">
    <child type="top">
      <object class="NautilusToolbar" id="toolbar">
        <property name="show-view-controls">true</property>
        <property name="show-history-controls">true</property>
        <property name="sidebar-button-active" bind-source="split_view" bind-property="show-sidebar" bind-flags="bidirectional|sync-create"/>
        <property name="window-slot" bind-source="NautilusWindow" bind-property="active-slot" bind-flags="sync-create"/>
      </object>
    </child>
    <child type="top">
      <object class="AdwTabBar" id="tab_bar">
        <property name="view">tab_view</property>
      </object>
    </child>
    <property name="content">
      <object class="AdwOverlaySplitView" id="inner_split_view">
        <property name="max-sidebar-width">400</property>
        <property name="sidebar-width-fraction">0.3</property>
        <property name="show-sidebar" bind-source="NautilusWindow" bind-property="show-info-panel" bind-flags="bidirectional|sync-create"/>
        <property name="content">
          <object class="AdwTabView" id="tab_view">
            <property name="menu-model">tab_menu_model</property>
          </object>
        </property>
        <property name="sidebar">
          <object class="NautilusInfoPanel" id="info_panel" />
        </property>
      </object>
    </property>
```

### src/nautilus-window.h

**Add to struct:**
```c
/* In NautilusWindow struct */
GtkWidget *info_panel;
gboolean show_info_panel;
```

### src/nautilus-window.c

**Add function:**
```c
static void
on_view_selection_changed (NautilusFilesView *view,
                           NautilusWindow *self)
{
    GList *selection = nautilus_files_view_get_selection (view);
    NautilusFile *file = NULL;

    if (selection != NULL && g_list_length (selection) == 1) {
        file = NAUTILUS_FILE (selection->data);
    }

    nautilus_info_panel_set_file (
        NAUTILUS_INFO_PANEL (self->info_panel),
        file
    );
}

static gboolean
show_info_panel_action (GtkWidget *widget,
                        GVariant *parameter,
                        gpointer user_data)
{
    NautilusWindow *self = NAUTILUS_WINDOW (user_data);

    self->show_info_panel = !self->show_info_panel;

    g_settings_set_boolean (nautilus_preferences,
                            "show-info-panel",
                            self->show_info_panel);

    gtk_widget_set_visible (self->info_panel, self->show_info_panel);

    return TRUE;
}
```

**In nautilus_window_init():**
```c
/* Setup action for toggling info panel */
g_action_map_add_action_entries (
    G_ACTION_MAP (self),
    (const GActionEntry[]) {
        {
            .name = "toggle-info-panel",
            .activate = show_info_panel_action,
        },
    },
    1,
    self
);

/* Connect to view selection changes */
g_signal_connect (self->active_view,
                  "selection-changed",
                  G_CALLBACK (on_view_selection_changed),
                  self);
```

### data/org.gnome.nautilus.gschema.xml

**Add keys:**
```xml
<key name="show-info-panel" type="b">
  <default>false</default>
  <summary>Show the information panel</summary>
  <description>
    If true, the information panel showing file details and preview
    will be displayed on the right side of the window.
  </description>
</key>

<key name="info-panel-width" type="i">
  <default>250</default>
  <summary>Width of the information panel</summary>
  <description>
    The width of the information panel in pixels.
  </description>
</key>
```

### src/nautilus-toolbar.c

**Add info panel toggle button to toolbar:**
```c
/* In toolbar creation */
GtkToggleButton *info_panel_button = gtk_toggle_button_new ();
gtk_button_set_icon_name (GTK_BUTTON (info_panel_button),
                          "sidebar-show-right-symbolic");
gtk_widget_set_tooltip_text (GTK_WIDGET (info_panel_button),
                             _("Toggle Info Panel"));
gtk_actionable_set_action_name (
    GTK_ACTIONABLE (info_panel_button),
    "win.toggle-info-panel"
);

gtk_box_append (GTK_BOX (self->end_box), info_panel_button);
```

### src/meson.build

**Add new source files:**
```meson
nautilus_sources = files(
    # ... existing files ...
    'nautilus-info-panel.c',
    'nautilus-info-panel.h',
)
```

## 3. Animation Mode Preference Integration

### Connect preference change signal in nautilus-info-panel.c

```c
static void
on_animation_mode_changed (GSettings *settings,
                           const char *key,
                           gpointer user_data)
{
    NautilusInfoPanel *self = NAUTILUS_INFO_PANEL (user_data);

    if (self->animation_paintable != NULL) {
        NautilusAnimationMode mode =
            nautilus_animated_thumbnail_get_mode ();

        if (mode == NAUTILUS_ANIMATION_MODE_NEVER) {
            nautilus_animated_paintable_stop (self->animation_paintable);
        } else if (!nautilus_animated_paintable_is_playing (
                       self->animation_paintable)) {
            nautilus_animated_paintable_start (self->animation_paintable);
        }
    }
}
```

## 4. Metadata Display Helper Functions

### Helper functions in nautilus-info-panel.c

```c
static void
add_metadata_row (NautilusInfoPanel *self,
                  const char *label,
                  const char *value)
{
    if (value == NULL || g_strcmp0 (value, "") == 0) {
        return;
    }

    GtkBox *row = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);

    GtkLabel *label_widget = gtk_label_new (label);
    gtk_label_set_markup (GTK_LABEL (label_widget),
                         g_markup_printf_escaped ("<b>%s</b>", label));
    gtk_widget_set_halign (GTK_WIDGET (label_widget), GTK_ALIGN_START);
    gtk_label_set_xalign (GTK_LABEL (label_widget), 0);

    GtkLabel *value_widget = gtk_label_new (value);
    gtk_widget_set_halign (GTK_WIDGET (value_widget), GTK_ALIGN_END);
    gtk_label_set_xalign (GTK_LABEL (value_widget), 1);
    gtk_label_set_wrap (GTK_LABEL (value_widget), TRUE);
    gtk_widget_set_hexpand (GTK_WIDGET (value_widget), TRUE);

    gtk_box_append (row, GTK_WIDGET (label_widget));
    gtk_box_append (row, GTK_WIDGET (value_widget));

    gtk_list_box_row_set_selectable (
        GTK_LIST_BOX_ROW (row),
        FALSE
    );

    /* Add to appropriate section */
    gtk_list_box_append (
        self->general_list_box,
        GTK_WIDGET (row)
    );
}

static void
format_permissions (guint mode,
                    char *buf,
                    size_t buf_size)
{
    /* Format as rwxr-xr-x */
    snprintf (buf, buf_size, "%c%c%c%c%c%c%c%c%c",
              S_ISDIR(mode) ? 'd' : '-',
              mode & S_IRUSR ? 'r' : '-',
              mode & S_IWUSR ? 'w' : '-',
              mode & S_IXUSR ? 'x' : '-',
              mode & S_IRGRP ? 'r' : '-',
              mode & S_IWGRP ? 'w' : '-',
              mode & S_IXGRP ? 'x' : '-',
              mode & S_IROTH ? 'r' : '-',
              mode & S_IWOTH ? 'w' : '-');
}

static void
clear_metadata_section (NautilusInfoPanel *self)
{
    GtkWidget *child;

    while ((child = gtk_widget_get_first_child (
                GTK_WIDGET (self->general_list_box))) != NULL) {
        gtk_list_box_remove (self->general_list_box, child);
    }
}
```

## 5. Keyboard Shortcut

### Add to data/gtk/help-overlay.ui

```xml
<item>
  <property name="title" translatable="yes">Toggle Info Panel</property>
  <property name="action-name">win.toggle-info-panel</property>
</item>
```

Or use keyboard shortcut binding:
```c
gtk_widget_add_shortcut (GTK_WIDGET (self),
                         gtk_shortcut_new (
                             gtk_keyval_trigger_new (GDK_KEY_i,
                                                      GDK_CONTROL_MASK | GDK_MOD1_MASK),
                             gtk_action_activate_action (
                                 gtk_property_action_new ("show-info-panel", ...)
                             )
                         ));
```

## 6. Build Integration

### Compile Check

```bash
# From ~/dev/nautilus-fork
meson compile -C build
```

### Common Build Issues

**Issue:** `undefined reference to nautilus_info_panel_new`
**Fix:** Make sure source files are added to `src/meson.build`

**Issue:** GTK template compilation errors
**Fix:** Ensure proper UI file registration in `meson.build`

**Issue:** Missing animation symbols
**Fix:** Verify `nautilus-animated-thumbnail.c` compiled successfully

## 7. Testing Checklist

```bash
# Build
meson compile -C build

# Run with debugging
./build/src/nautilus -c

# Test scenarios:
1. [ ] Click toggle button, panel appears/disappears
2. [ ] Select file, panel shows metadata
3. [ ] Select image, preview loads
4. [ ] Select video, animated preview plays
5. [ ] Select multiple files, panel clears
6. [ ] Change animation preference, playback updates
7. [ ] Restart app, panel state is restored
8. [ ] Resize panel divider smoothly
9. [ ] Panel scrolls when metadata overflows
10. [ ] Panel hides on window minimize
```
