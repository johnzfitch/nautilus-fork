# Building Nautilus for Omarchy

Complete build instructions for compiling and installing the Omarchy fork of Nautilus.

## Prerequisites

### System Requirements

- Arch Linux (or Arch-based distribution like Omarchy)
- 4GB RAM minimum
- 2GB free disk space

### Build Dependencies

```bash
# Install build tools
sudo pacman -S base-devel meson ninja gcc clang git

# Install Nautilus dependencies
sudo pacman -S \
  gtk4 libadwaita glib2 \
  localsearch tinysparql \
  gvfs gnome-desktop-4 \
  tracker3 tracker3-miners \
  libgexiv2 libportal libportal-gtk4 \
  gnome-autoar graphene \
  gst-plugins-base-libs gstreamer \
  cairo pango gdk-pixbuf2 \
  dconf libcloudproviders

# Optional dependencies
sudo pacman -S \
  libgsf  # For office files metadata
  poppler-glib  # For PDF thumbnails
  ffmpegthumbnailer  # For video thumbnails
  gdk-pixbuf2-webp  # For WebP support
```

## Quick Build

```bash
cd ~/dev/nautilus-fork

# Configure
meson setup build \
  --prefix=/usr \
  --buildtype=release

# Build
ninja -C build

# Install
sudo ninja -C build install

# Restart Nautilus
nautilus -q
```

## Development Build

For active development with debugging:

```bash
# Clean any existing build
rm -rf build

# Configure with debug symbols
meson setup build \
  --prefix=/usr \
  --buildtype=debug \
  -Dtests=true \
  -Ddocs=true \
  -Db_sanitize=address,undefined

# Build
ninja -C build

# Run tests
ninja -C build test

# Install locally
sudo ninja -C build install

# Run with debug output
NAUTILUS_DEBUG=all nautilus
```

## Build Options

### Meson Configuration Options

```bash
# List all options
meson configure build

# Common options:
-Dtests=true|false          # Enable/disable tests
-Ddocs=true|false           # Enable/disable documentation
-Dprofile=default|development  # Build profile
-Dextensions=true|false     # Enable extension support
```

### Build Types

```bash
# Release build (optimized, no debug symbols)
meson setup build --buildtype=release

# Debug build (debug symbols, no optimization)
meson setup build --buildtype=debug

# Debugoptimized (debug symbols + optimization)
meson setup build --buildtype=debugoptimized

# Plain (no special flags)
meson setup build --buildtype=plain
```

## Building with Custom Features

### Animated Thumbnail Support

```bash
# Ensure WebP/GIF support is available
sudo pacman -S gdk-pixbuf2-webp libwebp

# Build with animated thumbnail feature
git checkout feature/animated-webp-thumbnails

meson setup build \
  --prefix=/usr \
  --buildtype=release \
  -Dtests=true

ninja -C build
sudo ninja -C build install
```

### Omarchy Theme Integration

The Omarchy theme integration is automatic if you have the Omarchy theme system installed.

Verify integration:

```bash
# Check for Omarchy theme files
ls ~/.config/omarchy/current/theme/

# Nautilus reads GTK4 theme from:
gsettings get org.gnome.desktop.interface gtk-theme
gsettings get org.gnome.desktop.interface color-scheme
```

## Testing

### Running Unit Tests

```bash
# All tests
ninja -C build test

# Specific test suite
meson test -C build nautilus:file

# With verbose output
meson test -C build --verbose

# Run failed tests only
meson test -C build --no-rebuild --only-failed
```

### Code Coverage

```bash
# Build with coverage
meson configure build -Db_coverage=true
ninja -C build clean
ninja -C build test

# Generate coverage report
ninja -C build coverage-html

# View report
xdg-open build/meson-logs/coveragereport/index.html
```

### Memory Leak Detection

```bash
# Build with sanitizers
meson configure build -Db_sanitize=address,leak,undefined

# Rebuild
ninja -C build

# Run with leak detection
./build/src/nautilus

# Or with valgrind
valgrind --leak-check=full --show-leak-kinds=all \
  ./build/src/nautilus
```

## Creating Packages

### PKGBUILD for AUR

Create a custom PKGBUILD:

```bash
cd ~/dev/nautilus-fork

cat > PKGBUILD << 'EOF'
# Maintainer: Zack <zack@omarchy.dev>
pkgname=nautilus-omarchy
pkgver=49.1
pkgrel=1
pkgdesc="Nautilus file manager with Omarchy customizations"
arch=('x86_64')
url="https://github.com/johnzfitch/nautilus-fork"
license=('GPL3')
depends=(
  'gtk4' 'libadwaita' 'glib2'
  'localsearch' 'tinysparql'
  'gvfs' 'gnome-desktop-4'
  'libgexiv2' 'libportal' 'libportal-gtk4'
  'gnome-autoar' 'graphene'
  'gst-plugins-base-libs' 'gstreamer'
)
makedepends=('meson' 'ninja' 'gcc')
provides=('nautilus')
conflicts=('nautilus')
source=("git+https://github.com/johnzfitch/nautilus-fork.git#branch=main")
sha256sums=('SKIP')

build() {
  cd nautilus-fork

  meson setup build \
    --prefix=/usr \
    --buildtype=release \
    -Dtests=true

  ninja -C build
}

check() {
  cd nautilus-fork
  ninja -C build test || warning "Tests failed"
}

package() {
  cd nautilus-fork
  DESTDIR="$pkgdir" ninja -C build install
}
EOF

# Build package
makepkg -si

# Clean build artifacts
makepkg --clean
```

### Local Installation Without Package Manager

```bash
# Install to custom prefix
meson setup build --prefix=/opt/nautilus-omarchy
ninja -C build install

# Add to PATH
echo 'export PATH="/opt/nautilus-omarchy/bin:$PATH"' >> ~/.bashrc

# Run custom version
/opt/nautilus-omarchy/bin/nautilus
```

## Installation

### System-Wide Installation

```bash
cd ~/dev/nautilus-fork

# Build
meson setup build --prefix=/usr --buildtype=release
ninja -C build

# Install (replaces system Nautilus)
sudo ninja -C build install

# Restart Nautilus
nautilus -q

# Verify version
nautilus --version
```

### User-Local Installation

```bash
# Install to ~/.local
meson setup build --prefix=$HOME/.local --buildtype=release
ninja -C build install

# Update desktop database
update-desktop-database ~/.local/share/applications

# Run local version
~/.local/bin/nautilus
```

## Uninstallation

### Remove Custom Build

```bash
cd ~/dev/nautilus-fork

# Uninstall
sudo ninja -C build uninstall

# Reinstall official Nautilus
sudo pacman -S nautilus

# Clear cache
rm -rf ~/.cache/nautilus
rm -rf ~/.local/share/nautilus
```

## Troubleshooting

### Build Failures

#### Missing Dependencies

```bash
# Error: Package 'gtk4' not found
sudo pacman -S gtk4

# Error: Program 'msgfmt' not found
sudo pacman -S gettext

# Error: Dependency "localsearch-3.0" not found
sudo pacman -S localsearch
```

#### Meson Configuration Failed

```bash
# Clear build directory
rm -rf build

# Reconfigure with verbose output
meson setup build --wipe
```

#### Compilation Errors

```bash
# Update system
sudo pacman -Syu

# Reinstall base-devel
sudo pacman -S base-devel --needed

# Try different compiler
CC=clang meson setup build
```

### Runtime Issues

#### Nautilus Won't Start

```bash
# Check for errors
nautilus --verbose

# Check dependencies
ldd /usr/bin/nautilus

# Run with debug output
NAUTILUS_DEBUG=all G_MESSAGES_DEBUG=all nautilus
```

#### Thumbnails Not Working

```bash
# Clear thumbnail cache
rm -rf ~/.cache/thumbnails

# Verify thumbnail dependencies
pacman -Q | grep -E 'gdk-pixbuf|thumbnails|ffmpeg'

# Check LocalSearch status
systemctl --user status localsearch-3.service
```

#### Search Not Working

```bash
# Restart LocalSearch
systemctl --user restart localsearch-3.service

# Check search configuration
gsettings list-recursively org.freedesktop.Tracker3.Miner.Files

# Rebuild index
localsearch reset
systemctl --user restart localsearch-3.service
```

## Keeping Fork Updated

### Sync with Upstream

```bash
# Add upstream remote if not already added
git remote add upstream https://gitlab.gnome.org/GNOME/nautilus.git

# Fetch upstream changes
git fetch upstream

# Merge upstream main
git checkout main
git merge upstream/main

# Rebuild
ninja -C build clean
ninja -C build
sudo ninja -C build install
```

### Update Build Configuration

```bash
# Check for new dependencies
meson introspect build --dependencies

# Reconfigure if needed
meson configure build

# Or full reconfigure
meson setup build --wipe
```

## Performance Optimization

### Optimized Build Flags

```bash
# Maximum optimization
CFLAGS="-O3 -march=native -mtune=native" \
meson setup build --buildtype=release

# Link-time optimization
meson configure build -Db_lto=true

# Rebuild
ninja -C build clean
ninja -C build
```

### Profile-Guided Optimization (PGO)

```bash
# 1. Build with instrumentation
meson setup build-pgo --buildtype=release -Db_pgo=generate
ninja -C build-pgo

# 2. Run typical workload to generate profile data
./build-pgo/src/nautilus

# 3. Rebuild with profile data
meson configure build-pgo -Db_pgo=use
ninja -C build-pgo

# 4. Install optimized build
sudo ninja -C build-pgo install
```

## CI/CD Integration

### GitHub Actions Build

See `.github/workflows/build.yml` for automated builds.

### Local CI Test

```bash
# Simulate CI build
docker run --rm -v $(pwd):/build archlinux:latest bash -c "
  pacman -Syu --noconfirm
  pacman -S --noconfirm base-devel meson ninja gtk4 libadwaita localsearch
  cd /build
  meson setup build --prefix=/usr
  ninja -C build
  ninja -C build test
"
```

## Resources

- **Meson Documentation:** https://mesonbuild.com/
- **GTK4 Documentation:** https://docs.gtk.org/gtk4/
- **GNOME Development:** https://developer.gnome.org/
- **Arch Build System:** https://wiki.archlinux.org/title/Arch_Build_System

## Support

For build issues specific to Omarchy:
- GitHub Issues: https://github.com/johnzfitch/nautilus-fork/issues
- Omarchy Discord: https://discord.gg/omarchy

For upstream Nautilus issues:
- GNOME GitLab: https://gitlab.gnome.org/GNOME/nautilus/issues
