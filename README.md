# Nautilus-Plus

Enhanced GNOME Files (Nautilus) with performance optimizations and extended features.

**Repository:** https://github.com/johnzfitch/nautilus-plus
**AUR Package:** [nautilus-plus](https://aur.archlinux.org/packages/nautilus-plus)
**Based on:** GNOME Nautilus 50.alpha

## Features

- **Fast Search** - Sub-millisecond search via search-cache integration (2.3M+ files indexed)
- **Larger Thumbnails** - HUGE (384px) and GIGANTIC (512px) zoom levels for media files
- **Search Results Limit** - Configurable limit (default 1000) prevents memory exhaustion
- **FUSE Mount Detection** - 1-second timeout prevents UI hangs on stale remote mounts
- **Crash Prevention** - Lifecycle management fixes prevent use-after-free crashes

## Installation (Arch Linux)

### Option 1: AUR Package (Recommended)

```bash
yay -S nautilus-plus search-cache
systemctl --user enable --now search-cache
```

### Option 2: Build from Source

```bash
git clone https://github.com/johnzfitch/nautilus-plus
cd nautilus-plus
meson setup build
ninja -C build
sudo ninja -C build install
```

## ⚠️ IMPORTANT: search-cache Requirement

**Nautilus-Plus requires search-cache daemon to be running for fast search.**

Without search-cache:
- ❌ Search will be **very slow** (recursively scans every file)
- ❌ High CPU usage during search
- ❌ Fans may spin up

With search-cache:
- ✅ **Sub-millisecond search** (uses trigram index)
- ✅ Low CPU usage
- ✅ Instant results

### Verify search-cache is Running

```bash
systemctl --user status search-cache
```

Should show: `Active: active (running)`

If not running:
```bash
systemctl --user start search-cache
systemctl --user enable search-cache  # Auto-start on boot
```

### Check Indexed Files

```bash
sc stats
```

Should show your file count (e.g., "Files indexed: 2313792")

## Configuration

### Search Results Limit

Open **Preferences → Performance** and adjust "Search Results Limit"
- Default: 1000
- Range: 0-10000 (0 = unlimited, not recommended)

Or via command line:
```bash
gsettings set org.gnome.nautilus.preferences search-results-limit 2000
```

### Larger Thumbnails

Use the zoom slider or:
```bash
# Set to GIGANTIC (512px)
gsettings set org.gnome.nautilus.icon-view default-zoom-level 6
```

Zoom levels:
- 0 = Small (48px)
- 1 = Small Plus (64px)
- 2 = Medium (96px)
- 3 = Large (168px)
- 4 = Extra Large (256px)
- 5 = HUGE (384px) ⭐ NEW
- 6 = GIGANTIC (512px) ⭐ NEW

## Performance

**Memory Usage:**
- Small directory (100 files): ~3.8 MB
- Medium directory (1,000 files): ~3.8 MB
- Large directory (10,000 files): ~3.8 MB

**Search Performance** (with search-cache):
- 2.3M files: <1ms
- No CPU spike
- Instant results

## Troubleshooting

### Search is slow / high CPU usage

**Cause:** search-cache daemon not running

**Fix:**
```bash
systemctl --user start search-cache
systemctl --user enable search-cache
```

### No search results

**Cause:** search-cache index needs rebuilding

**Fix:**
```bash
sc rebuild
```

### Search-cache not starting

Check the service logs:
```bash
journalctl --user -u search-cache -n 50
```

Common issues:
- Index corruption → Run `sc rebuild`
- Permission issues → Check `~/.local/share/search-cache/`
- Out of memory → Reduce index size or increase swap

## Documentation

- [Installation Guide](INSTALLATION.md)
- [Search Blacklist Guide](docs/search-blacklist.md)
- [Detailed Installation](docs/installation.md)

## Upstream

Nautilus-Plus is a fork of [GNOME Nautilus](https://gitlab.gnome.org/GNOME/nautilus).

For upstream issues, use the [GNOME issue tracker](https://gitlab.gnome.org/GNOME/nautilus/issues).

For nautilus-plus specific issues, use [GitHub Issues](https://github.com/johnzfitch/nautilus-plus/issues).

## License

GPL-3.0-or-later (same as upstream GNOME Nautilus)
