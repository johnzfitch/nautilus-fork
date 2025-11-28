# Omarchy Nautilus Custom Features

<img src=".github/assets/icons/folder.png" width="24" height="24" align="left" style="margin-right: 8px;">

This document tracks all custom features in the Omarchy Nautilus fork to ensure proper merge conflict resolution during upstream syncs.

<br clear="left"/>

## Feature Registry

### <img src=".github/assets/icons/document.png" width="20" height="20"> 1. Location Shadow (List View)

| | |
|---|---|
| **Status** | <img src=".github/assets/icons/tick.png" width="16" height="16"> Implemented |
| **Added** | 2024-11 |
| **Merge Risk** | <img src=".github/assets/icons/warning.png" width="16" height="16"> Low-Medium |

Shows file's parent directory path at 60% opacity nested under filename in list view.
Long paths truncated to `.../last/three/components/`.

**Files Modified:**
| File | Change Type | Conflict Risk |
|------|-------------|---------------|
| `src/nautilus-name-cell.c` | Added struct member, helper function, update to `update_labels()` | Medium |
| `src/nautilus-name-cell.h` | Added function declaration | Low |
| `src/nautilus-list-view.c` | Added single function call in `setup_name_cell()` | Low |
| `src/resources/ui/nautilus-name-cell.ui` | Added `location_shadow` GtkLabel widget | Low |
| `src/resources/style.css` | Added `.location-shadow` CSS class | Low |

**Merge Strategy:**
- If `nautilus-name-cell.c` struct changes: Re-add `location_shadow` and `show_location_shadow` members
- If `update_labels()` is refactored: Re-integrate location shadow logic
- UI template conflicts: XML is additive, should merge cleanly
- CSS conflicts: Append to end of file

**Key Code Blocks to Preserve:**
```c
// In struct _NautilusNameCell:
GtkWidget *location_shadow;
gboolean show_location_shadow;

// Helper function:
static gchar *get_truncated_location(NautilusFile *file)

// In update_labels():
if (self->show_location_shadow) { ... }

// Public function:
void nautilus_name_cell_show_location_shadow(NautilusNameCell *self)
```

---

### <img src=".github/assets/icons/search.png" width="20" height="20"> 2. Search Cache Integration

| | |
|---|---|
| **Status** | <img src=".github/assets/icons/tick.png" width="16" height="16"> Implemented |
| **Added** | 2024-11 |
| **Merge Risk** | <img src=".github/assets/icons/warning.png" width="16" height="16"> Low-Medium |

Integrates the `sc` (search-cache) tool as Nautilus's primary search provider.
Provides sub-millisecond file search using a pre-built trigram index.

**Files Modified:**
| File | Change Type | Conflict Risk |
|------|-------------|---------------|
| `src/nautilus-search-engine-searchcache.c` | New file (search provider) | None |
| `src/nautilus-search-engine-searchcache.h` | New file (header) | None |
| `src/nautilus-search-engine.c` | Added provider initialization | Medium |
| `src/nautilus-search-engine.h` | Added search type enum | Low |
| `src/meson.build` | Added source files | Low |

**Merge Strategy:**
- New files won't conflict
- If `nautilus-search-engine.c` is refactored: Re-add provider setup in `setup_provider()` calls
- If search type enum changes: Re-add `NAUTILUS_SEARCH_TYPE_SEARCHCACHE`

**Key Code Blocks to Preserve:**
```c
// In nautilus-search-engine.h enum:
NAUTILUS_SEARCH_TYPE_SEARCHCACHE = 1 << 4,

// In struct _NautilusSearchEngine:
NautilusSearchProvider *searchcache;

// In nautilus_search_engine_set_search_type():
setup_provider (self, &self->searchcache, NAUTILUS_SEARCH_TYPE_SEARCHCACHE,
                (CreateFunc) nautilus_search_engine_searchcache_new);
```

**Dependencies:**
- Requires `sc` binary from search-and-filter project
- Falls back gracefully if `sc` not found

---

### <img src=".github/assets/icons/folder.png" width="20" height="20"> 3. Animated WebP/GIF/APNG Thumbnails

| | |
|---|---|
| **Status** | In Progress (Phase 2A) |
| **Added** | 2024-10 |
| **Merge Risk** | <img src=".github/assets/icons/warning.png" width="16" height="16"> Medium |

Enables animated thumbnail playback in grid/list views.

**Files Modified:**
| File | Change Type | Conflict Risk |
|------|-------------|---------------|
| `src/nautilus-file.c` | Thumbnail handling | Medium |
| `src/nautilus-grid-cell.c` | Animation playback | Medium |
| Various UI templates | Animation widgets | Low |

**Merge Strategy:**
- See `ANIMATED-WEBP-PLAN.md` for detailed merge notes
- Animation code is mostly additive
- Watch for thumbnail pipeline changes in upstream

---

### <img src=".github/assets/icons/folder.png" width="20" height="20"> 4. Rust Info Panel (Experimental)

| | |
|---|---|
| **Status** | Experimental |
| **Added** | 2024-11 |
| **Merge Risk** | <img src=".github/assets/icons/tick.png" width="16" height="16"> Low (isolated) |

GTK4 info panel written in Rust.

**Files Modified:**
| File | Change Type | Conflict Risk |
|------|-------------|---------------|
| `nautilus-info-panel-rs/` | New directory (isolated) | None |

**Merge Strategy:**
- Fully isolated, no upstream conflicts possible
- May need meson.build updates if integrated

---

## Upstream Sync Checklist

When merging upstream changes:

### Pre-Merge
- [ ] Read upstream changelog for breaking changes
- [ ] Check if modified files have upstream changes
- [ ] Back up custom feature code blocks

### During Merge
- [ ] Resolve conflicts file by file
- [ ] Preserve all custom struct members
- [ ] Preserve all custom functions
- [ ] Preserve UI template additions
- [ ] Preserve CSS additions

### Post-Merge
- [ ] Rebuild: `ninja -C build`
- [ ] Test location shadow displays correctly
- [ ] Test search-cache returns results
- [ ] Test animated thumbnails (if enabled)
- [ ] Run automated tests: `ninja -C build test`

### Conflict Resolution Priority
1. **Struct definitions** - Re-add members manually
2. **Function implementations** - Re-integrate logic
3. **Function calls** - Re-add calls at appropriate locations
4. **UI templates** - XML usually merges cleanly
5. **CSS** - Append to end if conflicts

---

## Files to Watch in Upstream

These files are modified by Omarchy and should be reviewed carefully during syncs:

### High Priority (Custom Logic)
- `src/nautilus-name-cell.c`
- `src/nautilus-name-cell.h`
- `src/nautilus-list-view.c`
- `src/nautilus-search-engine.c`
- `src/nautilus-search-engine.h`
- `src/nautilus-file.c`
- `src/nautilus-grid-cell.c`

### Medium Priority (UI/Style)
- `src/resources/ui/nautilus-name-cell.ui`
- `src/resources/style.css`
- `src/resources/style-hc.css`

### Low Priority (Build)
- `src/meson.build`
- `meson.build`

---

## Recovery Procedures

### If Location Shadow Breaks After Merge

1. Check struct definition in `nautilus-name-cell.c`:
   ```c
   struct _NautilusNameCell {
       // ... ensure these exist:
       GtkWidget *location_shadow;
       gboolean show_location_shadow;
   };
   ```

2. Check template binding in class_init:
   ```c
   gtk_widget_class_bind_template_child (widget_class, NautilusNameCell, location_shadow);
   ```

3. Check UI template has the widget:
   ```xml
   <object class="GtkLabel" id="location_shadow">
   ```

4. Check CSS class exists:
   ```css
   .location-shadow { opacity: 0.6; ... }
   ```

### If Search Cache Breaks After Merge

1. Verify `sc` is in PATH:
   ```bash
   which sc
   ```

2. Check search type enum includes searchcache:
   ```c
   NAUTILUS_SEARCH_TYPE_SEARCHCACHE = 1 << 4,
   ```

3. Check provider is initialized in `nautilus_search_engine_set_search_type()`

4. Test with debug logging:
   ```bash
   G_MESSAGES_DEBUG=nautilus-search nautilus
   ```

### If Build Fails After Merge

1. Clean build: `rm -rf build && meson setup build --prefix=/usr`
2. Check meson.build for missing sources
3. Check for renamed/removed upstream files
4. Review compile errors for API changes

---

## Version Compatibility

| Omarchy Feature | Minimum Nautilus | Maximum Tested |
|-----------------|------------------|----------------|
| Location Shadow | 46.0 | 50.alpha |
| Search Cache | 46.0 | 50.alpha |
| Animated Thumbs | 46.0 | 50.alpha |
| Rust Info Panel | 46.0 | 50.alpha |

---

## Contributing

When adding new custom features:

1. Add entry to this document
2. List all modified files with conflict risk
3. Document key code blocks to preserve
4. Add to upstream sync checklist
5. Test merge with upstream before committing
