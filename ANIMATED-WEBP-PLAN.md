# Animated WebP Thumbnail Support for Nautilus

## Current Status

### Option 1: Static Thumbnails (IN PROGRESS)
- **Script**: `~/.local/bin/webp-to-nautilus-thumbs.sh`
- **Status**: Running - 54% complete (10,200 / 18,796 files)
- **What it does**: Extracts first frame from animated WebP files and places them in Nautilus thumbnail cache
- **Result**: Static thumbnails will appear in Nautilus for all video files

### Option 3: Fork & Patch Nautilus (PLANNED)
Add native animated WebP thumbnail support to Nautilus

## Current Nautilus Thumbnail Architecture

### Key Files
- `src/nautilus-thumbnails.c` - Thumbnail generation logic
- `src/nautilus-thumbnails.h` - Thumbnail API
- `src/nautilus-previewer.c` - Preview/sidecar functionality

### How Thumbnails Currently Work
1. **Library**: Uses `libgnome-desktop/gnome-desktop-thumbnail.h`
2. **Image Loading**: Uses `GdkPixbuf` (via `gdk_pixbuf_get_formats()`)
3. **Sizes**: Supports 256px, 512px, 1024px thumbnails
4. **Storage**: MD5-hashed PNG files in `~/.cache/thumbnails/`
5. **Threading**: Async generation with thread pool
6. **MIME Types**: Determined by GdkPixbuf supported formats

### Why Animated Thumbnails Don't Work
- Nautilus only uses `GdkPixbuf` (static images)
- No `GdkPixbufAnimation` support
- Thumbnails are cached as static PNGs
- No animation state management (play/pause)

## Proposed Patch Strategy

### Phase 1: Detection & Loading
1. **Detect animated images**
   - Check if file format supports animation (WebP, GIF, APNG)
   - Use `gdk_pixbuf_animation_new_from_file()` instead of `gdk_pixbuf_new_from_file()`
   - Add flag to `NautilusFile` to mark animated files

2. **Modify thumbnail cache**
   - Store animated thumbnails separately or with special marker
   - Option A: Cache as animated WebP
   - Option B: Cache both static (default) and animated versions

### Phase 2: Display & Animation
1. **Grid/List View**
   - Add `GtkPicture` widget support (GTK4) or animation iter
   - Play animation on mouse hover OR file selection
   - Pause when not hovered/selected
   - Memory management: unload animations when not visible

2. **Animation State**
   - Track which thumbnails are playing
   - Limit concurrent animations (performance)
   - Add preference: "Animated Thumbnails: On Hover / On Select / Always / Never"

### Phase 3: Integration
1. **Settings**
   - Add gsettings key: `org.gnome.nautilus.preferences.animated-thumbnails`
   - Options: "never", "on-hover", "on-select"

2. **Performance**
   - Lazy load animations (only decode when needed)
   - Cache decoded frames in memory
   - Limit animation framerate (e.g., 10fps max)

## Technical Challenges

### 1. GdkPixbufAnimation vs Static
```c
// Current (static)
GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(path, NULL);

// Proposed (animated)
GdkPixbufAnimation *animation = gdk_pixbuf_animation_new_from_file(path, NULL);
if (gdk_pixbuf_animation_is_static_image(animation)) {
    // Handle as static
} else {
    // Handle as animated
}
```

### 2. Animation Playback
- Use `GdkPixbufAnimationIter` for frame-by-frame control
- Timer/timeout to advance frames
- Connect to widget visibility events

### 3. Memory Usage
- Animated WebPs use more RAM when decoded
- Need to limit number of concurrent animations
- Implement LRU cache for decoded animations

## Files to Modify

### Core Files
1. `src/nautilus-thumbnails.c`
   - Add animation detection
   - Modify thumbnail loading logic
   - Add animation cache management

2. `src/nautilus-file.c`
   - Add animated flag/property
   - Track animation state

3. `src/nautilus-canvas-item.c` or `src/nautilus-list-view.c`
   - Modify rendering to support animations
   - Add hover/selection event handlers

4. `data/org.gnome.nautilus.gschema.xml`
   - Add animated-thumbnails preference

### New Files
1. `src/nautilus-animated-thumbnail.c/h`
   - Animation manager
   - Playback control
   - Memory management

## Build Requirements

### Dependencies (already present)
- GTK4 / GTK3
- GdkPixbuf
- libgnome-desktop

### Additional Checks
- Ensure GdkPixbuf was compiled with WebP support
- Check for `libwebp` and `gdk-pixbuf-webp` loader

## Testing Plan

1. **Unit Tests**
   - Animation detection
   - Frame extraction
   - Cache management

2. **Integration Tests**
   - Large directories with mixed animated/static
   - Memory usage under load
   - Performance benchmarks

3. **User Testing**
   - Accessibility (can users disable?)
   - Performance on slow systems
   - Battery impact on laptops

## Timeline Estimate

- **Phase 1** (Detection & Loading): 2-4 hours
- **Phase 2** (Display & Animation): 4-6 hours
- **Phase 3** (Integration & Polish): 2-3 hours
- **Testing & Debugging**: 3-4 hours

**Total**: 11-17 hours of development

## Next Steps

1. ✅ Complete static thumbnail sync (Option 1)
2. Set up Nautilus build environment
3. Create feature branch: `feature/animated-webp-thumbnails`
4. Implement Phase 1: Animation detection
5. Test basic animated loading
6. Implement Phase 2: Playback on selection
7. Add preference controls
8. Submit patch upstream to GNOME

## Notes

- Consider submitting this as an upstream patch to GNOME
- May need design approval from GNOME design team
- Could be controversial (performance concerns)
- Alternative: Keep as local fork
