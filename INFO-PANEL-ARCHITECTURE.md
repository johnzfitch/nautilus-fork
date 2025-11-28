# Nautilus Info Panel - Architecture & Visual Reference

## Window Layout Transformation

### BEFORE (Current)
```
┌─────────────────────────────────────────────────┐
│ 📁 Files - Omarchy                              │
├──────────┬──────────────────────────────────────┤
│ Places   │ Toolbar (navigation, view options)   │
│ ────     ├──────────────────────────────────────┤
│ • Home   │ Tab Bar                              │
│ • Recent │ ├──────────────────────────────────┤
│ • Dev    │ │ 📄 📄 📄 📄 📄                    │
│ • Desktop│ │ index.html, main.rs, style.css   │
│ • Trash  │ │ config.toml, Cargo.toml          │
│          │ │                                  │
│          │ ├──────────────────────────────────┤
│          │ Files View (grid or list)          │
│          │ (takes full right side)            │
│          │                                    │
│          │                                    │
│          │                                    │
│          │                                    │
└──────────┴──────────────────────────────────────┘
```

### AFTER (With Info Panel)
```
┌─────────────────────────────────────────────────────────────┐
│ 📁 Files - Omarchy          [🖥️ Toggle Info Panel]          │
├──────────┬─────────────────────────────────┬────────────────┤
│ Places   │ Toolbar (navigation, view opts) │ INFO PANEL BTN │
│ ────     ├─────────────────────────────────┤────────────────┤
│ • Home   │ Tab Bar                         │                │
│ • Recent │ ├──────────────────────────────┤ ┌────────────┐ │
│ • Dev    │ │ 📄 📄 📄 📄 📄              │ │  PREVIEW   │ │
│ • Desktop│ │ index.html, main.rs, ...   │ │  ┌──────┐  │ │
│ • Trash  │ │ config.toml, Cargo.toml   │ │  │ 🎬   │  │ │
│          │ ├──────────────────────────────┤ │ 256×256│  │ │
│          │ Files View (split 70/30)      │ │ └──────┘  │ │
│          │                               │ └────────────┘ │
│          │                               │                │
│          │                               │ ─────────────  │
│          │                               │                │
│          │                               │ GENERAL INFO   │
│          │                               │ ─────────────  │
│          │                               │ Name: video.mp4│
│          │                               │ Type: Video    │
│          │                               │ Size: 45.2 MiB │
│          │                               │ Modified:      │
│          │                               │   2024-11-20   │
│          │                               │                │
│          │                               │ ⊟ PERMISSIONS │
│          │                               │                │
│          │                               │ ⊟ DETAILS      │
└──────────┴─────────────────────────────────┴────────────────┘
```

## Component Integration Flow

```
┌────────────────────────────────────────────────────────────┐
│                    NautilusWindow                          │
│  (nautilus-window.c / nautilus-window.ui)                 │
└────────┬─────────────────────────────────────────────────┬─┘
         │                                                 │
         ▼                                                 ▼
  ┌──────────────────┐                     ┌──────────────────────┐
  │  NautilusSidebar │                     │  AdwToolbarView      │
  │  (Left Sidebar)  │                     │  (Main Content Area) │
  │                  │                     │                      │
  │  • Places        │                     │  ┌────────────────┐  │
  │  • Bookmarks     │                     │  │ NautilusToolbar│  │
  └──────────────────┘                     │  └────────────────┘  │
                                            │  ┌──────────────────┐ │
                                            │  │ AdwOverlaySplit  │ │
                                            │  │ View (NEW)       │ │
                                            │  │                  │ │
                                            │  ├─────┬──────────┤ │
                                            │  │ Tab │ INFO     │ │ │
                                            │  │ View│ PANEL    │ │ │
                                            │  │ 70% │ (NEW)    │ │ │
                                            │  │     │ 30%      │ │ │
                                            │  │     │          │ │ │
                                            │  │     │ ┌──────┐ │ │
                                            │  │     │ │Prevw │ │ │
                                            │  │     │ │Video │ │ │
                                            │  │     │ │ 256px│ │ │
                                            │  │     │ └──────┘ │ │
                                            │  │     │          │ │
                                            │  │     │ ─────    │ │
                                            │  │     │ Metadata │ │
                                            │  │     │ - Name   │ │
                                            │  │     │ - Size   │ │
                                            │  │     │ - Type   │ │
                                            │  │     │ - Dates  │ │
                                            │  │     │          │ │
                                            │  └─────┴──────────┘ │
                                            └──────────────────────┘
```

## Animation Playback Integration

```
File Selection → Update Info Panel
    │
    ├─→ Detect MIME Type
    │   │
    │   ├─→ Video (.mp4, .webm, .mkv)
    │   │   └─→ nautilus_animated_thumbnail_is_supported()
    │   │       │
    │   │       ├─→ YES: Load Animation
    │   │       │   │
    │   │       │   └─→ nautilus_animated_thumbnail_load()
    │   │       │       │
    │   │       │       └─→ GdkPixbufAnimation
    │   │       │           │
    │   │       │           └─→ nautilus_animated_paintable_new()
    │   │       │               │
    │   │       │               └─→ NautilusAnimatedPaintable
    │   │       │                   │
    │   │       │                   ├─→ gtk_picture_set_paintable()
    │   │       │                   │
    │   │       │                   └─→ nautilus_animated_paintable_start()
    │   │       │                       (based on mode preference)
    │   │       │
    │   │       └─→ NO: Load Static Thumbnail
    │   │           └─→ gdk_pixbuf_new_from_file()
    │   │
    │   ├─→ Image (.jpg, .png, .gif, .webp)
    │   │   └─→ gdk_pixbuf_new_from_file()
    │   │       └─→ gtk_image_set_from_pixbuf()
    │   │
    │   └─→ Other Files
    │       └─→ Get MIME Icon
    │           └─→ gtk_image_set_from_icon_name()
    │
    └─→ Load Metadata
        │
        └─→ Update Metadata Display
            ├─→ General (Name, Type, Size)
            ├─→ Dates (Modified, Created, Accessed)
            ├─→ Permissions (Owner, Group, Mode)
            └─→ Expandable Sections
```

## File Structure

### New Files to Create
```
src/
├── nautilus-info-panel.h        (NEW)
├── nautilus-info-panel.c        (NEW)
└── resources/ui/
    └── nautilus-info-panel.ui   (NEW - optional, if using GTK template)
```

### Files to Modify
```
src/
├── nautilus-window.h            (MODIFY - add info_panel property)
├── nautilus-window.c            (MODIFY - selection handling)
├── nautilus-toolbar.h           (MODIFY - add toggle button)
├── nautilus-toolbar.c           (MODIFY - add toggle button)
└── resources/ui/
    └── nautilus-window.ui       (MODIFY - add inner split view + info panel)

data/
└── org.gnome.nautilus.gschema.xml (MODIFY - add preferences)

src/meson.build                  (MODIFY - add new source files)
```

## Data Flow for File Selection

```
User clicks file in browser
         │
         ▼
NautilusFilesView
  .selection-changed signal
         │
         ▼
NautilusWindow
  on_view_selection_changed()
         │
         ├─→ Get selected file(s)
         │
         ├─→ If exactly 1 file selected:
         │   │
         │   └─→ nautilus_info_panel_set_file(panel, file)
         │       │
         │       ├─→ Store NautilusFile reference
         │       │
         │       ├─→ Load Preview
         │       │   ├─→ Detect animation support
         │       │   └─→ Create paintable or load static
         │       │
         │       └─→ Update Metadata Display
         │           ├─→ nautilus_file_get_display_name()
         │           ├─→ nautilus_file_get_size()
         │           ├─→ nautilus_file_get_mime_type()
         │           ├─→ nautilus_file_get_modification_date()
         │           └─→ ... more metadata
         │
         └─→ If 0 or >1 files selected:
             │
             └─→ nautilus_info_panel_clear(panel)
                 └─→ Hide panel or show generic message
```

## Widget Hierarchy

```
NautilusInfoPanel (GtkBox)
│
├─ GtkScrolledWindow
│  │
│  └─ GtkBox (main_box, vertical)
│     │
│     ├─ GtkFrame (preview_frame)
│     │  │
│     │  └─ GtkBox (preview_box)
│     │     │
│     │     ├─ GtkPicture (animated_picture)
│     │     │  └─ [NautilusAnimatedPaintable]
│     │     │
│     │     └─ GtkImage (preview_image)
│     │        └─ [GdkPixbuf or Icon]
│     │
│     ├─ GtkSeparator
│     │
│     ├─ GtkBox (metadata_box)
│     │  │
│     │  ├─ GtkExpander (general_expander) "General"
│     │  │  └─ GtkListBox
│     │  │     ├─ GtkBox
│     │  │     │  ├─ GtkLabel "Name"
│     │  │     │  └─ GtkLabel [filename]
│     │  │     ├─ GtkBox
│     │  │     │  ├─ GtkLabel "Type"
│     │  │     │  └─ GtkLabel [mime type]
│     │  │     ├─ GtkBox
│     │  │     │  ├─ GtkLabel "Size"
│     │  │     │  └─ GtkLabel [human-readable size]
│     │  │     └─ ...
│     │  │
│     │  ├─ GtkSeparator
│     │  │
│     │  └─ GtkExpander (permissions_expander) "Permissions"
│     │     └─ GtkListBox
│     │        └─ ...
│     │
│     └─ GtkSeparator
│
└─ [empty space for scrolling]
```

## Animation Performance Considerations

```
Preview Size:     256×256 px (configurable)
Animation Quality: 10-30 fps (frame rate limited)
Memory per Video: ~5-10 MB (1 second clip)
Concurrent Limit: 1-2 animations playing
Update Strategy:  Lazy load (only when visible)
Cache:            LRU with size limit

Animation Lifecycle:
1. File selected
   │
2. Check if animated
   │
3. Load animation (if not cached)
   │
4. Create AnimatedPaintable
   │
5. Start playback (if mode != NEVER)
   │
6. Play until:
   - File deselected → Stop playback
   - Panel hidden    → Stop playback
   - Window closed   → Cleanup
```

## Preferences Flow

```
User toggles info panel button
    │
    ▼
win.toggle-info-panel action
    │
    ├─→ Update inner_split_view.show-sidebar property
    │
    └─→ Save preference to GSettings
        org.gnome.nautilus.show-info-panel = true/false
        org.gnome.nautilus.info-panel-width = pixels

On next window open:
    │
    ├─→ Read GSettings
    │
    └─→ Restore state:
        ├─→ inner_split_view.show-sidebar = saved value
        └─→ inner_split_view.sidebar-width-fraction = saved width
```

## Key Features Summary

### Preview Panel
- 📸 Static image preview (jpg, png, gif, etc.)
- 🎬 Animated video preview (mp4, webm, animated gif/webp)
- 🎨 Thumbnail scaling (fit to 256×256)
- ⚙️ Animation mode preference (never/hover/select/always)

### Metadata Display
- 📝 File name and type
- 📦 File size (human-readable)
- 📅 Modification, creation, access dates
- 🔒 Permissions and ownership
- 📍 Full path with symlink detection

### User Controls
- ✕ Toggle button in toolbar
- ⌨️ Keyboard shortcut (optional)
- 💾 Persistent state (shown/hidden on next open)
- 🖱️ Resizable divider (smooth drag to adjust width)

### Performance
- ⚡ Lazy loading (load only when needed)
- 💾 Animation caching (avoid reloading)
- 🎯 Selection tracking (update on file change)
- 🔋 Power efficient (animations pause when hidden)
