# Nautilus Info Panel Implementation Plan

## Overview
Add a Dolphin-like info viewing panel to the right side of the Nautilus window showing file metadata, preview thumbnails (including animated videos), and detailed information.

## Current Architecture

### Existing Window Structure
```
AdwApplicationWindow (NautilusWindow)
  └─ AdwToastOverlay
      └─ AdwOverlaySplitView (split_view) [LEFT-RIGHT]
          ├─ sidebar: NautilusSidebar (places/bookmarks)
          └─ content: AdwToolbarView
              └─ AdwTabView (file browser)
```

### Already Implemented
- ✅ `nautilus-animated-paintable.c/h` - GTK4 animated paintable
- ✅ `nautilus-animated-thumbnail.c/h` - Animation support infrastructure
- ✅ Animation modes: NEVER, ON_HOVER, ON_SELECT, ALWAYS
- ✅ Animation detection and loading
- ✅ Animation caching system

## New Architecture

### Proposed Window Structure
```
AdwApplicationWindow (NautilusWindow)
  └─ AdwToastOverlay
      └─ AdwOverlaySplitView (outer_split_view) [LEFT-RIGHT]
          ├─ sidebar: NautilusSidebar (places/bookmarks)
          └─ content: AdwToolbarView
              └─ AdwOverlaySplitView (inner_split_view) [CONTENT-PANEL]
                  ├─ file_view: AdwTabView (existing)
                  └─ info_panel: NautilusInfoPanel (NEW)
```

This maintains current left sidebar while adding right info panel.

## Phase 1: Create Info Panel Widget

### Files to Create
1. **src/nautilus-info-panel.h** - Header file
2. **src/nautilus-info-panel.c** - Implementation

### NautilusInfoPanel Structure
```c
struct _NautilusInfoPanel {
    GtkWidget parent_instance;

    // Widgets
    GtkScrolledWindow *scrolled_window;
    GtkBox *main_box;

    // Preview section
    GtkFrame *preview_frame;
    GtkBox *preview_box;
    GtkImage *preview_image;  // Static image fallback

    // Animated video preview
    GtkPicture *animated_picture;
    NautilusAnimatedPaintable *animation_paintable;

    // Metadata sections
    GtkBox *metadata_box;
    GtkExpander *general_expander;
    GtkExpander *permissions_expander;

    // Current file being displayed
    NautilusFile *file;
    GCancellable *cancellable;
};
```

### Key Methods
```c
NautilusInfoPanel *nautilus_info_panel_new(void);
void nautilus_info_panel_set_file(NautilusInfoPanel *panel, NautilusFile *file);
void nautilus_info_panel_clear(NautilusInfoPanel *panel);
```

## Phase 2: Metadata Display

### Sections to Display

#### General Information
- File name
- File type (mime type)
- File size (human-readable)
- Modified date/time
- Created date/time (if available)
- Accessed date/time (if available)

#### Location
- Full path
- Symbolic link target (if applicable)

#### Image/Video Specific
- Dimensions (width × height)
- Duration (for videos)
- Color depth/bit depth
- Frame rate (for videos)

#### Permissions
- Owner
- Group
- Permissions (rwx format)
- Special bits (setuid, setgid, sticky)

#### Storage
- Space used on disk
- Number of hard links

## Phase 3: Preview System

### Preview Generation Priority
1. **Animated Videos (.mp4, .webm, .gif, .webp)**
   - Use `nautilus_animated_thumbnail_load()` to get GdkPixbufAnimation
   - Create NautilusAnimatedPaintable
   - Display in GtkPicture widget
   - Play based on animation mode preference

2. **Static Images (.jpg, .png, .tiff, etc.)**
   - Load with GdkPixbuf
   - Display in GtkImage widget
   - Scale to fit preview area (e.g., 256×256)

3. **Documents (.pdf, .txt, .odt, etc.)**
   - Use thumbnailer via GdkPixbuf
   - Display first page preview

4. **Unsupported/Generic Files**
   - Show MIME type icon
   - Fallback to file manager icon theme

### Preview Area Specifications
- Maximum dimensions: 256×256 pixels
- Aspect ratio: Maintain original (don't stretch)
- Background: Theme background color
- Padding: 12px
- Border: Subtle 1px border

## Phase 4: Window Layout Integration

### Modify nautilus-window.ui

Current structure:
```xml
<property name="content">
  <object class="AdwToolbarView">
    <child type="top">
      <object class="NautilusToolbar"/>
    </child>
    <property name="content">
      <object class="AdwTabView" id="tab_view"/>
    </property>
  </object>
</property>
```

New structure:
```xml
<property name="content">
  <object class="AdwToolbarView">
    <child type="top">
      <object class="NautilusToolbar">
        <!-- Add info-panel-toggle action here -->
      </object>
    </child>
    <property name="content">
      <object class="AdwOverlaySplitView" id="inner_split_view">
        <property name="max-sidebar-width">400</property>
        <property name="sidebar-width-fraction">0.3</property>
        <property name="content">
          <object class="AdwTabView" id="tab_view"/>
        </property>
        <property name="sidebar">
          <object class="NautilusInfoPanel" id="info_panel">
            <property name="visible">False</property>
          </object>
        </property>
      </object>
    </property>
  </object>
</property>
```

### Add Info Panel Toggle Button
- Location: Toolbar (right side, near view controls)
- Icon: `sidebar-show-right-symbolic` or `document-properties-symbolic`
- Tooltip: "Toggle Info Panel"
- Action: `win.toggle-info-panel`
- Keyboard shortcut: Could be `Ctrl+Alt+I` or similar

## Phase 5: Selection Handling

### Update nautilus-window.c
```c
static void
on_view_selection_changed(NautilusView *view,
                          NautilusWindow *self)
{
    GList *selected = nautilus_view_get_selection(view);
    NautilusFile *file = NULL;

    if (selected && g_list_length(selected) == 1) {
        file = NAUTILUS_FILE(selected->data);
    }

    nautilus_info_panel_set_file(
        NAUTILUS_INFO_PANEL(self->info_panel),
        file
    );
}
```

### Connect Signals
- When file selection changes → Update info panel
- When active tab changes → Update info panel
- When file properties change → Refresh info panel display

## Phase 6: Animation Support Integration

### In nautilus-info-panel.c
```c
static void
load_preview_for_file(NautilusInfoPanel *self,
                      NautilusFile *file)
{
    const char *mime_type = nautilus_file_get_mime_type(file);
    char *path = nautilus_file_get_path(file);

    // Check if file supports animation
    if (nautilus_animated_thumbnail_is_supported(mime_type)) {
        GdkPixbufAnimation *animation =
            nautilus_animated_thumbnail_load(path, NULL);

        if (animation != NULL) {
            NautilusAnimatedPaintable *paintable =
                nautilus_animated_paintable_new(animation);

            gtk_picture_set_paintable(
                GTK_PICTURE(self->animated_picture),
                GDK_PAINTABLE(paintable)
            );

            // Start animation based on mode
            NautilusAnimationMode mode =
                nautilus_animated_thumbnail_get_mode();

            if (mode != NAUTILUS_ANIMATION_MODE_NEVER) {
                nautilus_animated_paintable_start(paintable);
            }
        }
    } else {
        // Load static image
        load_static_preview(self, path, mime_type);
    }
}
```

## Phase 7: Preferences

### Add GSettings Keys
In `data/org.gnome.nautilus.gschema.xml`:
```xml
<key name="show-info-panel" type="b">
  <default>false</default>
  <summary>Show the information panel</summary>
  <description>
    If true, the information panel will be displayed on the right side
    of the window showing details about selected files.
  </description>
</key>

<key name="info-panel-width" type="i">
  <default>250</default>
  <summary>Width of the information panel</summary>
  <description>The width of the information panel in pixels</description>
</key>
```

### Persist State
- Save `show-info-panel` preference when toggled
- Restore on next window open
- Save panel width on resize

## Implementation Sequence

### Step 1: Create Info Panel Widget
- Create nautilus-info-panel.c/h
- Implement basic widget with scrollable box
- Add GTK4 template UI or build in code

### Step 2: Add Metadata Display
- Implement metadata gathering from NautilusFile
- Create display rows for each metadata type
- Add formatting (human-readable sizes, dates)

### Step 3: Add Preview Section
- Create preview frame at top
- Add GtkImage for static images
- Add GtkPicture for animated paintables
- Implement preview loading logic

### Step 4: Integrate with Window
- Modify nautilus-window.ui
- Update nautilus-window.c to handle panel
- Add toggle action and button

### Step 5: Add Animation Support
- Connect to nautilus-animated-paintable
- Load animations based on file type
- Respect animation mode preferences

### Step 6: Test and Polish
- Test with various file types
- Optimize preview performance
- Add accessibility features
- Handle edge cases

## Technical Considerations

### Performance
- Lazy-load previews (don't generate until panel visible)
- Cache thumbnailer results
- Limit concurrent animation playback
- Don't update if file hasn't changed

### Memory Management
- Unload animations when panel hidden
- Clear old file references
- Implement proper cleanup in destructor

### Accessibility
- Proper widget focus order
- Screen reader labels
- Keyboard navigation
- High contrast support

### Theming
- Use theme colors (background, foreground)
- Support dark mode
- Respect system font settings
- Scale properly on HiDPI displays

## Testing Plan

### Unit Tests
- Metadata extraction
- File format detection
- Preview generation
- Animation detection

### Integration Tests
- Panel with various file types
- Multi-file selection (show nothing or generic panel)
- Panel toggling
- Window resizing with panel

### User Scenarios
1. Open file, info panel shows details
2. Click different file, panel updates
3. Toggle panel off/on
4. Resize panel
5. Play animated video thumbnail
6. Restart app, panel state restored

## Estimation

- Info Panel Widget: 2 hours
- Metadata Display: 2 hours
- Preview System: 2 hours
- Window Integration: 1 hour
- Animation Integration: 1 hour
- Testing & Polish: 2 hours

**Total: 10 hours**

## Git Workflow

1. Create feature branch: `git checkout -b feature/info-panel`
2. Implement in phases with commits:
   - `Add NautilusInfoPanel widget skeleton`
   - `Implement metadata display`
   - `Add preview system with static images`
   - `Integrate animated paintable support`
   - `Add info panel to main window layout`
   - `Add toggle button and preferences`
3. Test thoroughly
4. Merge to main when complete

## Future Enhancements

- Drag-and-drop file operations from info panel
- Quick file operations (rename, delete from panel)
- Extended metadata for music files (artist, album, duration)
- Image histogram/color information
- Archive content preview
- Thumbnail grid view option
- Search result preview mode
