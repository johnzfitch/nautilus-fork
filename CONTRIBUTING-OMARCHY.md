# Contributing to Omarchy Nautilus Fork

This document describes how to contribute changes to our custom Nautilus fork for Omarchy.

## Repository Structure

```
nautilus-fork/
├── .github/workflows/
│   └── upstream-sync-with-ai-agents.yml  # Automated upstream sync
├── src/                                   # Nautilus source code
├── docs/                                  # Sync specifications & reviews
├── CONTRIBUTING-OMARCHY.md                # This file
├── BUILDING-OMARCHY.md                    # Build instructions
└── ANIMATED-WEBP-PLAN.md                  # Custom feature docs
```

## Custom Features

### 1. Animated Thumbnail Support
- **Branch:** `feature/animated-webp-thumbnails`
- **Files:** See `ANIMATED-WEBP-PLAN.md` and `PHASE2-STATUS.md`
- **Status:** In development
- **Features:**
  - Animated WebP thumbnails
  - Animated GIF thumbnails
  - Animated APNG thumbnails

### 2. Omarchy Theme Integration
- Custom color scheme integration
- Walker launcher compatibility
- LocalSearch indexing optimizations

## Development Workflow

### Setting Up Development Environment

```bash
# Clone the repository
git clone git@github.com:johnzfitch/nautilus-fork.git
cd nautilus-fork

# Add upstream remote
git remote add upstream https://gitlab.gnome.org/GNOME/nautilus.git
git fetch upstream

# Install build dependencies (Arch Linux)
sudo pacman -S meson ninja gcc gtk4 libadwaita \
               localsearch tinysparql gvfs \
               gnome-desktop-4 tracker3 tracker3-miners

# Build
meson setup build
ninja -C build

# Run tests
ninja -C build test

# Install locally
sudo ninja -C build install
```

### Making Changes

1. **Create a feature branch:**
   ```bash
   git checkout main
   git pull origin main
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes:**
   - Follow GNOME coding style
   - Add tests for new features
   - Update documentation

3. **Test your changes:**
   ```bash
   ninja -C build test
   nautilus --version  # Verify build
   ```

4. **Commit your changes:**
   ```bash
   git add -A
   git commit -m "feat: Add your feature description

   Detailed explanation of changes.
   Related to: #issue-number
   "
   ```

5. **Push and create PR:**
   ```bash
   git push origin feature/your-feature-name
   gh pr create --base main --title "feat: Your Feature"
   ```

## Commit Message Format

We use Conventional Commits:

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Types
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `chore`: Maintenance tasks
- `upstream`: Upstream merge commits

### Examples

```
feat(thumbnails): Add animated WebP support

Implement animated thumbnail playback for WebP images using
GdkPaintable and custom animation controller.

Closes: #123
```

```
upstream: Merge GNOME Nautilus 49.1

Merged upstream changes from commit abc123def.
Resolved conflicts in src/nautilus-view.c

Breaking changes:
- API change in NautilusFile
- New dependency: libfoo >= 1.2.3
```

## Upstream Sync Process

Our fork stays up-to-date with GNOME Nautilus through an automated AI agent pipeline.

### Automated Workflow

The GitHub Action `.github/workflows/upstream-sync-with-ai-agents.yml` runs every 6 hours:

1. **Check for upstream updates** from GNOME Nautilus
2. **@copilot generates specification** analyzing changes
3. **@claude implements merge** resolving conflicts
4. **@codex reviews code** for quality assurance
5. **Creates draft PR** for human review

### Manual Completion Steps

The workflow creates a draft PR but requires manual steps:

#### Step 1: Complete Copilot Analysis

```bash
# Check out the sync branch
git checkout upstream-sync-YYYYMMDD-HHMMSS

# Open the spec file
cat docs/UPSTREAM-SYNC-SPEC-*.md

# Use GitHub Copilot in your IDE to analyze
# Ask: "Analyze these upstream changes for breaking changes,
#       compatibility issues, and merge strategy"

# Save Copilot's analysis
echo "# Copilot Analysis" > docs/COPILOT-ANALYSIS-$(date +%Y%m%d).md
# Paste Copilot's response

git add docs/
git commit -m "spec: Add Copilot analysis"
git push
```

#### Step 2: Run Claude Implementation

```bash
# Ensure you're on the sync branch
git checkout upstream-sync-YYYYMMDD-HHMMSS

# Run Claude Code with the task file
claude --prompt "$(cat .claude-task.md)" --auto-approve --max-iterations 10

# Or manually:
# 1. Merge upstream
git merge upstream/main

# 2. Resolve conflicts (preserve our custom features!)
#    Priority files to review:
#    - src/nautilus-thumbnail-*
#    - Any files with animated thumbnail code

# 3. Run tests
ninja -C build test

# 4. Update docs
echo "## Changes in this sync" >> CHANGELOG.md
git log --oneline upstream/main..HEAD >> CHANGELOG.md

# 5. Commit
git add -A
git commit -m "feat: Merge upstream changes

Merged GNOME Nautilus commit $(git rev-parse upstream/main).
Resolved conflicts while preserving animated thumbnail features.

Ready for @codex review.
"

git push
```

#### Step 3: Run Codex Review

```bash
# Still on sync branch
git checkout upstream-sync-YYYYMMDD-HHMMSS

# Create review task
cat > .codex-review-task.md << 'EOF'
# Code Review Task

Review criteria:
1. Code quality (no memory leaks, proper error handling)
2. Custom features intact (animated thumbnails work)
3. Performance (no regressions)
4. Security (no new vulnerabilities)
5. Testing (all tests pass)
6. Documentation (changes documented)

Output format:
- ✅ Approved sections
- ⚠️ Warnings (minor issues)
- ❌ Blocking issues (must fix)
- 💡 Suggestions (improvements)
EOF

# Run codex review
codex review \
  --context .codex-review-task.md \
  --output docs/CODEX-REVIEW-$(date +%Y%m%d).md

# Commit review
git add docs/
git commit -m "review: Codex code review complete"
git push

# Mark PR as ready for review
gh pr ready
```

#### Step 4: Human Review & Merge

```bash
# Test in Omarchy environment
nautilus  # Verify UI works
# Test animated thumbnails
# Test search
# Test theme integration

# If all checks pass:
gh pr merge --squash --delete-branch
```

### Manual Upstream Sync

If you need to sync manually (without waiting for automation):

```bash
# Trigger the workflow manually
gh workflow run upstream-sync-with-ai-agents.yml \
  --field force_sync=true

# Or completely manual:
git checkout main
git pull origin main
git fetch upstream main

# Create sync branch
git checkout -b upstream-sync-$(date +%Y%m%d)

# Merge
git merge upstream/main

# Resolve conflicts
# Test
# Commit
# Push
# Create PR
```

## Testing

### Running Tests

```bash
# All tests
ninja -C build test

# Specific test
ninja -C build test/test-nautilus-file

# With coverage
meson configure build -Db_coverage=true
ninja -C build test
ninja -C build coverage
```

### Manual Testing Checklist

After making changes, test these areas:

- [ ] File browsing works
- [ ] Search works (LocalSearch integration)
- [ ] Thumbnails generate correctly
- [ ] Animated thumbnails play (WebP/GIF/APNG)
- [ ] Copy/paste/move operations work
- [ ] Properties dialog displays correctly
- [ ] Omarchy theme colors applied
- [ ] Walker integration functional
- [ ] No memory leaks (run with `valgrind`)
- [ ] No performance regressions

### Testing in Omarchy

```bash
# Build and install
ninja -C build install

# Restart Nautilus
nautilus -q
nautilus &

# Test animated thumbnails
cd ~/Pictures
# Create test images with animations
# Verify thumbnails animate in grid/list view

# Test search
# Press Ctrl+F
# Search for files
# Verify LocalSearch indexing works

# Test theme
# Change Omarchy theme
omarchy-theme-set catppuccin
# Verify Nautilus adopts new colors
```

## Building for Omarchy

See `BUILDING-OMARCHY.md` for detailed build instructions specific to Omarchy.

### Quick Build

```bash
# Clean build
rm -rf build
meson setup build \
  --prefix=/usr \
  --buildtype=release \
  -Dtests=true \
  -Ddocs=true

ninja -C build
sudo ninja -C build install

# Restart Nautilus
nautilus -q
```

### Creating AUR Package

```bash
# Generate PKGBUILD
cd ~/dev/nautilus-fork
./scripts/generate-pkgbuild.sh > PKGBUILD

# Build package
makepkg -si

# Test installed package
pacman -Qi nautilus-omarchy
```

## Debugging

### Enable Debug Logs

```bash
# Run with debug output
NAUTILUS_DEBUG=all nautilus

# Specific debug categories
NAUTILUS_DEBUG=thumbnails,search nautilus

# With GDB
gdb --args nautilus
(gdb) run
(gdb) bt  # After crash
```

### Common Issues

#### Thumbnails Not Animating

1. Check if GdkPaintable is working:
   ```bash
   NAUTILUS_DEBUG=thumbnails nautilus
   ```

2. Verify animated thumbnail files exist:
   ```bash
   ls ~/.cache/thumbnails/large/
   ```

3. Check for errors:
   ```bash
   journalctl -f | grep nautilus
   ```

#### Search Not Working

1. Verify LocalSearch is running:
   ```bash
   systemctl --user status localsearch-3.service
   ```

2. Check indexing status:
   ```bash
   localsearch status
   ```

3. Reindex if needed:
   ```bash
   systemctl --user restart localsearch-3.service
   ```

## Code Style

Follow GNOME coding style:

```c
/* Function documentation */
static void
nautilus_function_name (NautilusObject *object,
                        GtkWidget      *widget,
                        gpointer        user_data)
{
    g_return_if_fail (NAUTILUS_IS_OBJECT (object));

    if (condition)
    {
        /* Comment */
        do_something ();
    }
}
```

Use `clang-format` with GNOME style:

```bash
clang-format -i src/nautilus-file.c
```

## Documentation

### Updating Documentation

When adding features:

1. Update `README.md` if user-facing
2. Add technical docs to `docs/`
3. Update `CHANGELOG.md`
4. Add inline code comments
5. Update man pages if needed

### Documentation Files

- `README.md` - Main project README
- `CONTRIBUTING-OMARCHY.md` - This file
- `BUILDING-OMARCHY.md` - Build instructions
- `ANIMATED-WEBP-PLAN.md` - Animated thumbnail feature
- `docs/` - Technical specifications
- `man/` - Man pages

## Release Process

### Creating a Release

1. **Update version:**
   ```bash
   # Edit meson.build
   version: '49.1-omarchy.1'
   ```

2. **Update CHANGELOG.md:**
   ```markdown
   ## [49.1-omarchy.1] - 2025-01-20

   ### Added
   - Animated WebP thumbnail support
   - Custom Omarchy theme integration

   ### Changed
   - Improved LocalSearch integration

   ### Fixed
   - Fixed thumbnail generation crash
   ```

3. **Create release:**
   ```bash
   git tag -a v49.1-omarchy.1 -m "Release 49.1-omarchy.1"
   git push origin v49.1-omarchy.1
   ```

4. **Build packages:**
   ```bash
   ./scripts/build-release.sh v49.1-omarchy.1
   ```

5. **Create GitHub release:**
   ```bash
   gh release create v49.1-omarchy.1 \
     --title "Nautilus 49.1 Omarchy Edition 1" \
     --notes-file CHANGELOG.md \
     ./dist/*.tar.xz
   ```

## Getting Help

- **Issues:** https://github.com/johnzfitch/nautilus-fork/issues
- **Discussions:** https://github.com/johnzfitch/nautilus-fork/discussions
- **Upstream:** https://gitlab.gnome.org/GNOME/nautilus
- **Omarchy:** https://github.com/omarchy/omarchy

## License

Nautilus is licensed under GPL-3.0-or-later. See `LICENSE` file.
