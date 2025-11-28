# Omarchy Nautilus Fork - Complete Setup Guide

This document provides complete instructions for setting up, building, and maintaining the Omarchy fork of Nautilus with automated upstream synchronization.

## Overview

**Repository:** https://github.com/johnzfitch/nautilus-fork
**Upstream:** https://gitlab.gnome.org/GNOME/nautilus
**Current Version:** 49.1 + Omarchy customizations

### Custom Features

1. **Animated Thumbnail Support** - WebP, GIF, APNG animations
2. **Omarchy Theme Integration** - Seamless theme system integration
3. **LocalSearch Optimizations** - Improved search performance

### AI Agent Pipeline

Automated upstream sync uses three AI agents:
- **@copilot** - Specification & analysis
- **@claude** - Implementation & merge
- **@codex** - Code review & QA

---

## Initial Setup

### 1. Fork Repository (Already Done)

```bash
# Repository already exists at:
~/dev/nautilus-fork

# Remotes configured:
# - origin: git@github.com:johnzfitch/nautilus-fork.git
# - upstream: https://gitlab.gnome.org/GNOME/nautilus.git
```

### 2. Omarchy Integration

#### Remove Nautilus from Base Packages

**Status:** ✅ Completed

The nautilus package has been removed from `/home/zack/.local/share/omarchy/install/omarchy-base.packages` so `omarchy-update` won't overwrite your custom build.

Verify removal:

```bash
grep -n "nautilus" ~/.local/share/omarchy/install/omarchy-base.packages
# Should return no results
```

#### Prevent Package Manager Conflicts

```bash
# Mark nautilus as manually installed (not from repos)
sudo pacman -D --asexplicit nautilus

# Or completely remove official package
sudo pacman -Rdd nautilus  # Warning: breaks dependencies temporarily

# Then install your custom build
cd ~/dev/nautilus-fork
meson setup build --prefix=/usr
ninja -C build
sudo ninja -C build install
```

### 3. Configure GitHub Secrets

Set up required secrets in your repository settings:

```bash
# Go to: https://github.com/johnzfitch/nautilus-fork/settings/secrets/actions

# Add these secrets:
GH_PAT                 # GitHub Personal Access Token with repo scope
ANTHROPIC_API_KEY      # Claude API key (for future automation)
CODEX_API_KEY          # Your codex API key (optional)
```

Generate GitHub PAT:
```bash
# Using GitHub CLI
gh auth token

# Or manually:
# https://github.com/settings/tokens/new
# Scopes: repo, workflow
```

---

## Building Nautilus

### Quick Build & Install

```bash
cd ~/dev/nautilus-fork

# Configure
meson setup build --prefix=/usr --buildtype=release

# Build
ninja -C build

# Test
ninja -C build test

# Install
sudo ninja -C build install

# Restart Nautilus
nautilus -q
```

### Development Build

For active development with debugging:

```bash
rm -rf build

meson setup build \
  --prefix=/usr \
  --buildtype=debug \
  -Dtests=true \
  -Db_sanitize=address

ninja -C build
NAUTILUS_DEBUG=all ./build/src/nautilus
```

See `BUILDING-OMARCHY.md` for comprehensive build instructions.

---

## Automated Upstream Sync

### How It Works

The GitHub Action `.github/workflows/upstream-sync-with-ai-agents.yml`:

1. **Runs every 6 hours** checking for upstream updates
2. **Detects changes** by comparing commit hashes
3. **Creates sync branch** named `upstream-sync-YYYYMMDD-HHMMSS`
4. **Generates specification** documenting upstream changes
5. **Creates draft PR** with manual completion instructions

### Workflow Steps

#### Automated Steps (GitHub Actions)

- ✅ Check for upstream updates
- ✅ Create feature branch
- ✅ Generate changelog and diff
- ✅ Create specification document
- ✅ Create draft pull request

#### Manual Steps (You)

**Step 1: Copilot Analysis** (5 minutes)

```bash
# Checkout the sync branch
git fetch origin
git checkout upstream-sync-YYYYMMDD-HHMMSS

# Read the spec
cat docs/UPSTREAM-SYNC-SPEC-*.md

# Use GitHub Copilot to analyze
# In your IDE with Copilot:
# 1. Open docs/UPSTREAM-SYNC-SPEC-*.md
# 2. Ask Copilot: "Analyze these upstream changes for breaking changes,
#    compatibility with our animated thumbnail feature, and recommend
#    a merge strategy"
# 3. Save Copilot's response to docs/COPILOT-ANALYSIS-YYYYMMDD.md

git add docs/COPILOT-ANALYSIS-*.md
git commit -m "spec: Add Copilot analysis"
git push
```

**Step 2: Claude Implementation** (15-30 minutes)

```bash
# Still on sync branch
git checkout upstream-sync-YYYYMMDD-HHMMSS

# Option A: Use Claude Code (if available)
claude --prompt "$(cat .claude-task.md)" --auto-approve

# Option B: Manual implementation
# 1. Merge upstream
git merge upstream/main

# 2. Resolve conflicts
#    Priority files to review:
#    - src/nautilus-thumbnail-*
#    - src/nautilus-files-view.c
#    - Any animated paintable code

# 3. Test the build
meson setup build
ninja -C build test

# 4. Test manually
sudo ninja -C build install
nautilus -q
nautilus  # Verify animated thumbnails work

# 5. Update documentation
cat >> CHANGELOG.md << EOF

## Upstream Sync $(date +%Y-%m-%d)

Merged GNOME Nautilus commit $(git rev-parse upstream/main).

### Upstream Changes
$(git log --oneline HEAD..upstream/main | head -10)

### Compatibility
- ✅ Animated thumbnails preserved
- ✅ Omarchy theme integration intact
- ✅ LocalSearch optimizations maintained

EOF

# 6. Commit
git add -A
git commit -m "feat: Merge upstream GNOME Nautilus

Merged commit $(git rev-parse upstream/main).
Resolved conflicts preserving custom features.

Ready for @codex review.
"

git push
```

**Step 3: Codex Review** (5 minutes)

```bash
# Create review task
cat > .codex-review-task.md << 'EOF'
# Code Review: Upstream Sync

Review for:
- Code quality (no memory leaks)
- Custom features intact
- Performance (no regressions)
- Security (no vulnerabilities)
- Testing (all tests pass)

Output:
- ✅ Approved
- ⚠️ Warnings
- ❌ Blocking issues
- 💡 Suggestions
EOF

# Run codex
codex review \
  --context .codex-review-task.md \
  --output docs/CODEX-REVIEW-$(date +%Y%m%d).md

# Commit review
git add docs/
git commit -m "review: Codex code review complete"
git push

# Mark PR as ready
gh pr ready
```

**Step 4: Final Testing & Merge** (10 minutes)

```bash
# Test in Omarchy
ninja -C build test
sudo ninja -C build install
nautilus -q

# Manual testing:
# - Browse files
# - Test animated thumbnails
# - Test search (Ctrl+F)
# - Verify theme colors
# - Check performance

# If all good, merge
gh pr merge --squash --delete-branch

# Update local main
git checkout main
git pull origin main
```

### Manual Trigger

Force a sync check immediately:

```bash
# Using GitHub CLI
gh workflow run upstream-sync-with-ai-agents.yml \
  --field force_sync=true

# Or via web:
# https://github.com/johnzfitch/nautilus-fork/actions/workflows/upstream-sync-with-ai-agents.yml
```

---

## Daily Workflow

### Working on Features

```bash
# Create feature branch
git checkout main
git pull origin main
git checkout -b feature/my-awesome-feature

# Make changes
# ... edit files ...

# Build and test
ninja -C build
ninja -C build test

# Commit
git add -A
git commit -m "feat: Add my awesome feature

Detailed description of changes.
"

# Push and create PR
git push origin feature/my-awesome-feature
gh pr create \
  --base main \
  --title "feat: My Awesome Feature" \
  --body "Description of feature"
```

### Testing Changes

```bash
# Quick test
ninja -C build test

# Full test with installation
sudo ninja -C build install
nautilus -q
nautilus

# Test specific scenarios:
# - Animated thumbnails
# - Search functionality
# - Theme integration
# - Performance
```

### Keeping Fork Updated

```bash
# Fetch latest upstream
git fetch upstream main

# Check for updates
git log HEAD..upstream/main --oneline

# If updates exist, wait for automated PR
# Or manually trigger workflow
gh workflow run upstream-sync-with-ai-agents.yml --field force_sync=true
```

---

## Maintenance

### Regular Tasks

**Weekly:**
- Check for automated sync PRs
- Review and merge sync PRs
- Test latest build
- Update documentation if needed

**Monthly:**
- Review open issues
- Update dependencies
- Performance profiling
- Security audit

**Quarterly:**
- Major version planning
- Feature roadmap review
- Codebase cleanup
- Documentation overhaul

### Monitoring

#### Check Workflow Status

```bash
# View recent workflow runs
gh run list --workflow upstream-sync-with-ai-agents.yml

# View specific run
gh run view <run-id>

# View logs
gh run view <run-id> --log
```

#### Check for Failed Syncs

```bash
# List failed runs
gh run list --status failure

# View failure details
gh run view <failed-run-id> --log-failed
```

### Troubleshooting

#### Sync PR Has Merge Conflicts

```bash
git checkout upstream-sync-YYYYMMDD-HHMMSS
git merge main  # Merge latest main into sync branch
# Resolve conflicts
git commit
git push
```

#### Build Fails After Sync

```bash
# Check for new dependencies
meson introspect build --dependencies

# Install missing deps
sudo pacman -S <missing-package>

# Reconfigure
meson setup build --wipe
ninja -C build
```

#### Automated Tests Fail

```bash
# Run tests with verbose output
meson test -C build --verbose

# Fix failing tests
# Update test code if needed
git add test/
git commit -m "test: Fix failing tests after upstream sync"
```

---

## GitHub Actions Configuration

### Workflow File Location

```
.github/workflows/upstream-sync-with-ai-agents.yml
```

### Workflow Schedule

- **Cron:** `0 */6 * * *` (every 6 hours)
- **Manual:** Via workflow_dispatch

### Workflow Outputs

Each sync creates:
- `docs/UPSTREAM-SYNC-SPEC-YYYYMMDD-HHMMSS.md` - Changelog and analysis
- `docs/COPILOT-ANALYSIS-YYYYMMDD.md` - Copilot's spec (manual)
- `docs/CODEX-REVIEW-YYYYMMDD.md` - Codex review results (manual)
- `.claude-task.md` - Claude implementation instructions
- Draft Pull Request on GitHub

---

## Resources

### Documentation

- `README.md` - Main project README
- `CONTRIBUTING-OMARCHY.md` - Complete contribution guide
- `BUILDING-OMARCHY.md` - Comprehensive build instructions
- `ANIMATED-WEBP-PLAN.md` - Animated thumbnail feature docs
- `PHASE2-STATUS.md` - Current development status

### External Links

- **Fork Repository:** https://github.com/johnzfitch/nautilus-fork
- **Upstream Nautilus:** https://gitlab.gnome.org/GNOME/nautilus
- **GNOME GitLab:** https://gitlab.gnome.org/GNOME
- **Omarchy:** https://github.com/omarchy/omarchy

### Tools

- **GitHub CLI:** `gh` - https://cli.github.com/
- **Meson Build:** `meson` - https://mesonbuild.com/
- **Claude Code:** `claude` - (when available)
- **Codex:** `codex` - Your custom review tool

---

## Quick Reference

### Common Commands

```bash
# Build
ninja -C build

# Test
ninja -C build test

# Install
sudo ninja -C build install

# Restart Nautilus
nautilus -q

# Check for updates
git fetch upstream && git log HEAD..upstream/main

# Trigger sync
gh workflow run upstream-sync-with-ai-agents.yml --field force_sync=true

# View PRs
gh pr list

# View workflow runs
gh run list

# Check package status
pacman -Qi nautilus
```

### Emergency Rollback

If something breaks after a sync:

```bash
# Revert to previous version
git checkout main
git reset --hard HEAD^
git push origin main --force-with-lease

# Reinstall
meson setup build --wipe
ninja -C build
sudo ninja -C build install
nautilus -q
```

---

## Support

**For Omarchy-specific issues:**
- GitHub Issues: https://github.com/johnzfitch/nautilus-fork/issues
- Create issues for:
  - Build failures
  - Sync problems
  - Custom feature bugs
  - Documentation improvements

**For upstream Nautilus issues:**
- GNOME GitLab: https://gitlab.gnome.org/GNOME/nautilus/issues

**For general Omarchy help:**
- Omarchy Repository: https://github.com/omarchy/omarchy
- Omarchy Discord: (if available)

---

## Status

- ✅ Fork created and configured
- ✅ Automated sync workflow implemented
- ✅ Removed from omarchy-base.packages
- ✅ Documentation complete
- ⏳ Awaiting first automated sync PR
- ⏳ Testing AI agent pipeline

Last Updated: 2025-11-20
