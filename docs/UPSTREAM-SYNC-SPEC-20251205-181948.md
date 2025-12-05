# Upstream Sync Specification

Generated: 2025-12-05 18:19:48 UTC
Upstream Commit: ba493951c5a9e6d2568fb15fcd866d419543b387
Current Commit: 5d920f7387f09f8eefd35a118a512b183abcacba

## Upstream Changes

* ba493951c Update Slovenian translation
* 563b3547d application: Update DBus locations on window close
* 30b434222 window: Use g_return_if_fail over g_assert
* ec1a727b0 application: Rework dbus locations update mechanism
* 52d1777d8 application: Query list of window locations
* 5b19afe10 application: Simplify dbus_opened_locations() variables
* 3f70012c2 application: Use HashTable for opened locations
* 9b542917c window: Make slot_close private
* f2a25e5e6 application: Add reuse open flag
* 66b16db54 application: Use open_location_full to open new windows
* 379b7c145 application: Assure non-NULL locations in open call
* 4af2ce4d5 mime-actions: Drop slot from application_open call
* 459776d6a application: Simplify opening with target slot/window
* 4efbe2005 application: Simplify open flag check
* 9d94651a4 mime-actions: Drop unused functions
* 815771912 window: Inline usage of active slot
* 23ee8ec29 application: Query location from window
* bdf6a15a7 application: Simplify debug on location open
* 36ba41000 window: Make set_active_slot internal
* d2231f836 window: Explain close request return value
* 15079a65e window: Minor slot list optimization
* 95c864c57 window: Drop outdated comment
* 468d9912f window-slot: Sink reference on creation
* f56fe9ad1 sidebar: Add settings menu item to context menu for trash and recent
* bd02a6232 Pass activation_token to previewer (sushi)
* eba209e8b file-chooser: Strip newlines from file name
* 80dda1006 Update Romanian translation
* baaba69bc Update Romanian translation
* 95c234ce2 Update Turkish translation
* b848f1f50 Update Icelandic translation
* 562b5c449 Update Occitan translation
* 60b59d624 Update Indonesian translation
* e59038bfc mime-actions: Removing Non-xdg mimetypes
* a0c2d0d14 batch-rename-dialog: Use green highlight for replacement text
* ceea31451 dnd: Protect against empty file lists in drops
* 9eab30567 gcovr: Expand exclusion filters
* bf0eada04 test: Remove usage of RUNNING_TESTS env variable
* 264851868 test/bookmarks: Add tests for bookmarks
* bcd5175ef file: Fix a leak
* 08d7054e2 test/file-operations-create: Add tests for creation operations
* 2e953596a file-undo-operations: Fix new file redo
* 66244d9bb test/file-operations-link: Add tests for linking files
* 28afae88d test/file-operations-copy: Add file duplication tests
* dd8e35c09 test/file-operations-copy-files: Refactor test names
* adf58d959 test/file-operations-trash-or-delete: Reorganize test names
* 2f37665e1 test/files-view: Add tests for zoom actions
* ed08aa004 files-view: Add more accessors for tests
* b764ea2d4 test/files-view: Add a test for selection actions
* 845dd3d8b test/file: Add a basic test for directory deep counts and item count
* 1387ff6a2 test/display/sidebar: Add a basic test for sidebar
* 32a20bf97 test/file: Add tests for permissions
* df5f7cf9a test/utilities: Add a blocking version of nautilus_file_call_when_ready
* dca9aedaa test/files-view: Add test for renaming hidden files
* 6bacb75c1 query: Fix string handling
* fe8cf1409 progress-info: Fix short status leak
* d08750162 extensions: Fix list store leak
* 7b91cf0ac Revert "search-engine-model: Drop custom thread deduplication"
* 710d41f4a view-cell: Redraw when screen scale factor changes
* c1b488b27 files-view: Remove thumbnail attribute monitoring
* 53ddfa8cc file: Don't thumbnail when custom icons exist
* 8ce7771f0 properties-window: Don't depend on icon being already available
* 5a825cb94 grid-cell+name-cell: Use the new NautilusImage widget for loading thumbnails
* 558e7e39c image: Create a new const-size async loading image widget
* 7ec7416b0 thumbnails: Fix multiple requests for the same URI
* 5c32ccf00 Update Russian translation
* ee7ac5133 build: Use meson features for some options
* 0f21fd5cc search-engine-simple: Drop unused member variable
* 2c98e12de search-engine-model: Drop custom thread deduplication
* 33e03bfa0 query: Enhance mime type functionality
* 319dbc58a search-engine-simple: Use queue free_full function
* 85c28c21b search-engine-localsearch: Only warn about connection on creation
* 7877491e4 search-engine-simple: Simplify recursion check
* 6b771727c search-engine-simple: Simplify visited tracking snippet
* 57278c012 search-engine-simple: Simplify search thread function
* 77d209d88 search-engine-simple: Cleanup visit_directory variables
* 48b14676c search-engine-simple: Simplify show-hidden check
* 0492cd1fd search-engine-simple: Use g_file_enumerator_iterate
* 0f3f4a04d search-engine-simple: Add content type define
* 1fb98d992 files-view: Add shortcut hint for paste in context menu
* 2c2912a0c files-view: Add alternative bindings for copy and paste
* f60913bd3 search-engine: Clarify start counter variable name
* 1e2b31d3e search-provider: Simplify finished signal
* e5ed56546 search-test: Cleanup engine
* d93bab022 search-engine: Stop acting like a search provider
* 61f6b496f search-directory: Drop file list reset on restart
* 9d2f63f71 search-engine: Simplify restart logic
* 7c96f205e search-engine: Check providers directly after start
* 50e96741e search-tests: Drop stopping of engine
* 4ee128ccd search: Don't propagate errors
* b00ff21b2 shell-search: Fix error parameter handling
* 2397b3647 search-engine: Inline setup function
* 59a9d4b9a directory: Dissolve get_internal
* 39ba5a9fd directory: Simplify nautilus_directory_new
* dfc918943 file: Simplify get_internal logic
* 166f166a0 directory: Drop unused self-owned parameter
* fad86c14c file: Add dedicated self-owned constructor
* ef8f07ed8 directory: Add direct vfs file constructor
* def283d21 file: Make new_from_filename internal
* 9e6114432 directory: Drop unused filename from constructor
* 1777975b8 directory-private: Drop duplicate declaration
* eb6ce2720 file: Add dedicated FileInfo getter function
* 7ae2f7298 file: Use autocleanups in get_interal
* 75343142a directory: Use hash table destroy functions
* f9231491d directory: Simplify call_files_changed_common
* 83835c7b0 directory: Use files-added signal function
* 55a047cc7 directory: Adjust hash table list helper
* 6775e5daf directory: Use autocleanups in directory_moved
* 238668467 directory: Simplify directory hash table filling
* 1af72d660 directory: Simplify parent directory lookups
* dbaf97c96 directory: Add directories lookup helper
* 20d810e85 directory: Use autocleanups in notify functions
* 95970c264 directory: Fix directory reference leak
* b05578e6b directory: Initialize static directories hash table via helper
* b0e086160 Update Slovenian translation
* c6b6df53d thumbnail: Give arguments directly for nautilus_can_thumbnail()
* c5a69c6f4 thumbnails: Turn nautilus_create_thumbnail into an async function
* 3cfb2788c thumbnails: Move setting pixbuf on the file to the end
* 3bb744b8a thumbnails: Update thumbnailling status correctly
* fbe16f822 hash-queue: Drop key builder for key-value
* db01b0d47 file-operations: Cleanup macro usage
* 85044453a file: Use auto pointers
* fd4198994 file: Move list of conditions into a function
* 02ee2cff0 style: Fix inconsistant styling
* bd4b66d83 general: Run uncrustify
* c46af1457 Update Japanese translation
* a2410e6e1 bookmark-list: Fix uninitialized variable usage of index
* dc8d71159 Update Slovenian translation
* e6cd0d99d progress-info: Rename status property to short-status
* 665c98702 progress-info: Introduce short-status
* bc0d79b27 progress-info: Simplify set_status
* f5d0351fa progress-info: Only notify prop status change when status is changed
* d039ad481 test/search-engine-tracker: Reuse miner fs connection
* 07b4cad54 general: Adjust to localsearch App ID
* 3760f8d27 general: Use tinyspaql includes
* b6b0b45c3 sidebar: Invert loopback device sorting
* d39caa25f search-popover: Make file type filter buttons more accessible
* 25ce6bc23 search-popover: Allow multiple file type filters
* 6e0ce9eda batch-rename-utilities: Remove reordering function
* 8138f5f02 test/file: Add batch rename tests
* b577b39b8 test-utilities: Write the file name when creating a hierarchy
* 5c87e6921 file-undo-operation: Fix batch rename list leak
* c3dddce9e file: Don't update files in info query async callback
* d2a9a1763 file: Batch rename rework
* cb552854d file: Don't call callback on every invalid rename
* 5fa12ae9a file: Don't skip the first file from operation progress monitoring
* d2dd05573 file: Remove unncessary condition
* 8a0d12c7b name-cell: Fix uneven margin
* e71882d51 name-cell: Unset icon margin when switching to cut icon

## Files Changed

 .github/assets/icons/document.png                  |  Bin 948 -> 0 bytes
 .github/assets/icons/folder.png                    |  Bin 1279 -> 0 bytes
 .github/assets/icons/search.png                    |  Bin 1646 -> 0 bytes
 .github/assets/icons/tick.png                      |  Bin 922 -> 0 bytes
 .github/assets/icons/warning.png                   |  Bin 789 -> 0 bytes
 .github/workflows/aur-publish.yml                  |  188 -
 .github/workflows/build-and-release.yml            |  237 -
 .github/workflows/upstream-sync-with-ai-agents.yml |  206 -
 .github/workflows/upstream-sync.yml                |  215 -
 .gitignore                                         |    4 -
 ANIMATED-WEBP-PLAN.md                              |  174 -
 BUILDING-OMARCHY.md                                |  517 --
 CONTRIBUTING-OMARCHY.md                            |  548 --
 INFO-PANEL-ARCHITECTURE.md                         |  335 --
 INFO-PANEL-CODE-REFERENCE.md                       |  587 --
 INFO-PANEL-PLAN.md                                 |  402 --
 OMARCHY-FEATURES.md                                |  275 -
 OMARCHY-SETUP.md                                   |  600 ---
 PHASE2-STATUS.md                                   |  248 -
 build-aux/flatpak/org.gnome.Nautilus.json          |   14 +-
 data/org.gnome.nautilus.gschema.xml                |   24 +-
 .../audio-video-properties/totem-properties-view.c |    1 +
 gcovr.cfg                                          |    4 +-
 meson.build                                        |   12 +-
 meson_options.txt                                  |   10 +-
 nautilus-plus.install                              |   24 -
 po/id.po                                           | 2079 ++++----
 po/is.po                                           | 5590 ++++++++++++--------
 po/ja.po                                           | 2528 ++++-----
 po/oc.po                                           | 2109 ++++----
 po/ro.po                                           | 2141 ++++----
 po/ru.po                                           | 2115 ++++----
 po/sl.po                                           | 2631 ++++-----
 po/tr.po                                           |  952 ++--
 src/meson.build                                    |   11 +-
 src/nautilus-animated-paintable.c                  |  216 -
 src/nautilus-animated-paintable.h                  |   29 -
 src/nautilus-animated-thumbnail.c                  |  299 --
 src/nautilus-animated-thumbnail.h                  |   68 -
 src/nautilus-application.c                         |  348 +-
 src/nautilus-application.h                         |   13 +-
 src/nautilus-batch-rename-dialog.c                 |   27 +-
 src/nautilus-batch-rename-utilities.c              |  123 +-
 src/nautilus-batch-rename-utilities.h              |    9 +-
 src/nautilus-bookmark-list.c                       |   10 +-
 src/nautilus-directory-async.c                     |    6 +-
 src/nautilus-directory-private.h                   |    5 +-
 src/nautilus-directory.c                           |  534 +-
 src/nautilus-directory.h                           |    8 +-
 src/nautilus-enums.h                               |    1 +
 src/nautilus-file-chooser.c                        |   26 +-
 src/nautilus-file-operations.c                     |  146 +-
 src/nautilus-file-private.h                        |    6 +-
 src/nautilus-file-undo-operations.c                |   27 +-
 src/nautilus-file-utilities.c                      |  182 -
 src/nautilus-file-utilities.h                      |   23 -
 src/nautilus-file.c                                |  633 ++-
 src/nautilus-files-view.c                          |   25 +-
 src/nautilus-files-view.h                          |    4 +
 src/nautilus-global-preferences.h                  |    7 -
 src/nautilus-grid-cell.c                           |   78 +-
 src/nautilus-hash-queue.c                          |   32 +-
 src/nautilus-hash-queue.h                          |   11 +-
 src/nautilus-icon-info.c                           |    4 +-
 src/nautilus-image.c                               |  846 +++
 src/nautilus-image.h                               |   34 +
 src/nautilus-list-view.c                           |    3 -
 src/nautilus-localsearch-utilities.c               |    4 +-
 src/nautilus-localsearch-utilities.h               |    2 +-
 src/nautilus-mime-actions.c                        |   67 +-
 src/nautilus-mime-actions.h                        |    7 -
 src/nautilus-name-cell.c                           |  192 +-
 src/nautilus-name-cell.h                           |    1 -
 src/nautilus-network-directory.c                   |   16 +-
 src/nautilus-preferences-dialog.c                  |  292 -
 src/nautilus-previewer.c                           |   35 +-
 src/nautilus-progress-info.c                       |   83 +-
 src/nautilus-progress-info.h                       |    7 +-
 src/nautilus-properties-window.c                   |   19 +-
 src/nautilus-query-editor.c                        |   20 +-
 src/nautilus-query.c                               |   62 +-
 src/nautilus-query.h                               |    8 +-
 src/nautilus-search-directory.c                    |   87 +-
 src/nautilus-search-engine-localsearch.c           |   90 +-
 src/nautilus-search-engine-model.c                 |   20 +-
 src/nautilus-search-engine-recent.c                |   26 +-
 src/nautilus-search-engine-searchcache.c           |  335 --
 src/nautilus-search-engine-searchcache.h           |   24 -
 src/nautilus-search-engine-simple.c                |  275 +-
 src/nautilus-search-engine.c                       |  203 +-
 src/nautilus-search-engine.h                       |   11 +-
 src/nautilus-search-hit.c                          |   21 +-
 src/nautilus-search-popover.c                      |  153 +-
 src/nautilus-search-provider.c                     |   37 +-
 src/nautilus-search-provider.h                     |   41 +-
 src/nautilus-shell-search-provider.c               |   45 +-
 src/nautilus-sidebar.c                             |   85 +-
 src/nautilus-starred-directory.c                   |   16 +-
 src/nautilus-tag-manager.c                         |    2 +-
 src/nautilus-thumbnails.c                          |  275 +-
 src/nautilus-thumbnails.h                          |   15 +-
 src/nautilus-toolbar.c                             |    2 +-
 src/nautilus-vfs-file.c                            |    4 +-
 src/nautilus-view-item.c                           |    8 -
 src/nautilus-view-model.c                          |   18 -
 src/nautilus-window-slot.c                         |    8 +-
 src/nautilus-window.c                              |  234 +-
 src/nautilus-window.h                              |   17 +-
 src/resources/gtk/help-overlay.ui                  |    6 +-
 src/resources/style.css                            |    9 +-
 src/resources/ui/nautilus-grid-cell.ui             |    4 +-
 src/resources/ui/nautilus-name-cell.ui             |  102 +-
 src/resources/ui/nautilus-preferences-dialog.ui    |   54 -
 src/resources/ui/nautilus-progress-indicator.ui    |    2 +-
 src/resources/ui/nautilus-search-popover.ui        |   10 +
 test/automated/display/meson.build                 |    4 +
 .../display/test-file-operations-create.c          |  303 ++
 test/automated/display/test-files-view.c           |  262 +-
 test/automated/display/test-sidebar.c              |   52 +
 test/automated/displayless/meson.build             |    2 +
 test/automated/displayless/test-bookmarks.c        |  105 +
 .../displayless/test-file-operations-copy-files.c  |  240 +-
 .../displayless/test-file-operations-link.c        |  297 ++
 .../test-file-operations-trash-or-delete.c         |   37 +-
 test/automated/displayless/test-file.c             |  380 ++
 .../test-nautilus-search-engine-localsearch.c      |   24 +-
 .../test-nautilus-search-engine-model.c            |   20 +-
 .../test-nautilus-search-engine-simple.c           |   20 +-
 .../displayless/test-nautilus-search-engine.c      |   21 +-
 test/automated/displayless/test-thumbnails.c       |  213 +-
 test/automated/test-utilities.c                    |   28 +
 test/automated/test-utilities.h                    |    2 +
 test/meson.build                                   |    1 -
 133 files changed, 15572 insertions(+), 18057 deletions(-)

## AI Agent Tasks

1. @copilot: Analyze breaking changes and compatibility
2. @claude: Implement merge and resolve conflicts
3. @codex: Code review and QA
