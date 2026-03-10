# Changelog - vantisCorp/OBS---Test-unofficial

All notable changes to this fork will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased] - fix/add-extra-cmake-modules branch

### Fixed
- **CodeQL Workflow**: Added all missing build dependencies and CMake flags
- **clang-format**: Formatted all 18 modified files to pass CI checks
- **Tests Workflow**: Added `libxkbcommon-dev`, `uthash-dev`, `libjansson-dev` dependencies
- **Benchmark Build**: Fixed compilation errors (const char*, unused variables, missing includes)
- **CMakeLists.txt**: Added benchmark subdirectory for build inclusion
- **End-of-file**: Added missing newlines to 20+ source/header files

### Changed
- **README.rst**: Fixed repository URLs and added CI badges
- **CHANGELOG_VANTIS.md**: Updated with missing version entries
- **CodeQL workflow**: Full dependency parity with tests workflow

### Added
- **SECURITY.md**: Security policy and vulnerability reporting guidelines
- **CODEOWNERS**: Code ownership definitions for review routing
- **.github/dependabot.yml**: Automated dependency update configuration

## [32.0.4-vantis.11] - 2026-03-08

### Fixed
- **CI/CD**: Removed hardcoded Windows SDK version for CI compatibility (PR #12)

## [32.0.4-vantis.10] - 2026-03-08

### Fixed
- **CI/CD**: Install SIMDe from GitHub for Linux builds
- **CI/CD**: Added `simde-dev` dependency for SIMD Everywhere support
- **CI/CD**: Added `extra-cmake-modules` dependency for Linux builds

### Changed
- Updated CHANGELOG with vantis.8 and vantis.9 releases

## [32.0.4-vantis.9] - 2026-03-08

### Added
- **CI/CD Documentation**: Added `docs/CI-CD-REQUIREMENTS.md` explaining billing requirements
- **Minimal CI Workflow**: Added `.github/workflows/ci-minimal.yaml` with status reporting

### Changed
- **Workflow Runners**: Updated all workflows to use `*-latest` runners
  - `ubuntu-24.04` → `ubuntu-latest`
  - `macos-15` → `macos-latest`
  - `windows-2022` → `windows-latest`
- **Xcode Version**: Updated from non-existent `Xcode_26.1` to `Xcode_15.2`
- **README**: Updated for vantisCorp fork with links to upstream

### Fixed
- **Source Render Cache**: Added missing `memory-pool-fixed.h` and test files

### Documentation
- Documented that private repos require GitHub Pro/Team for Actions
- Added solutions: upgrade plan, make public, or self-hosted runners

## [32.0.4-vantis.8] - 2026-03-08

### Added
- **Source Render Cache**: Cached source properties for hot path optimization
- **Unit Tests**: 25 CMocka tests for source cache

### Changed
- Integrated cache into `render_video()` in `libobs/obs-source.c`

## [32.0.4-vantis.7] - 2026-03-08

### Added
- **Lock-Free SPSC Queue**: Single producer single consumer queue using atomics
- **Unit Tests**: 11 tests including multi-threaded producer-consumer

## [32.0.4-vantis.6] - 2026-03-08

### Added
- **SIMD Audio Integration**: Integrated SIMD into `mix_audio()` hot path

## [32.0.4-vantis.5] - 2026-03-08

### Added
- **Faza 4 - SIMD Audio Mixing**
  - SIMD-optimized audio mixing functions (SSE/AVX/NEON)
  - 4x-8x faster audio mixing on supported platforms
  - Unit tests for all mixing functions
- **Cleanup**: Removed old patch branches (24.0.4-patch, 25.0.8-patch)
- **Docs**: Added Phase 4 development plan (FAZA4_PLAN.md)

### Changed
- Updated CI/CD workflow permissions for private repo compatibility
- Changed ubuntu-24.04 to ubuntu-latest for better runner availability

## [32.0.4-vantis.4] - 2026-03-08

### Added
- **Phase 4 Development Plan**: Added `FAZA4_PLAN.md` with hot path optimizations roadmap

### Changed
- Updated CI/CD workflow permissions for private repo compatibility
- Changed `ubuntu-24.04` to `ubuntu-latest` for better runner availability

### Documentation
- Updated CHANGELOG with phases 1-3 and vantis.3 release

## [32.0.4-vantis.3] - 2026-03-08

### Added
- **Faza 3 - Wydajność projektu**
  - Benchmark suite (42 benchmarki) dla obs_data, obs_source, obs_audio, obs_video
  - Memory pool implementation dla hot paths pre-allocation
  - HOT_PATHS.md documentation z identyfikacją krytycznych ścieżek
  - CI/CD workflow dla automatycznych benchmarków
- **Feature: High precision floating point images**
  - Wsparcie dla 16-bit half-float (RGBAF16LE)
  - Wsparcie dla 32-bit float (RGBAF32LE)
  - TIFF image handling improvements

### Fixed
- **Memory pool improvements** (PR #4)
  - Usunięcie static global variable z header file
  - Meyers' singleton pattern dla C global state
  - Explicit C++ casts dla type safety
  - Unit tests (9 testów, wszystkie przechodzą)

## [32.0.4-vantis.2] - 2026-03-08

### Added
- **Faza 2 - Jakość projektu**
  - Code quality improvements
  - Testing infrastructure
  - Documentation updates

### Changed
- Improved code formatting and style consistency

## [32.0.4-vantis.1] - 2026-03-07

### Added
- **Faza 1 - Fundamenty projektu**
  - GitHub Issue templates (bug report, feature request)
  - CI/CD workflow for automated testing (`tests.yaml`)
  - Pre-commit hooks configuration for code quality
  - This CHANGELOG file

### Fixed
- **Critical**: Fix null pointer dereference in RTMP error handling (issue #13144)
  - Added null check for `description` parameter in `PublisherAuth()` function
  - Added guard before calling `PublisherAuth()` when description field is missing
  - Prevents segfault when RTMP server sends error without description field

### Changed
- Improved error logging for missing RTMP error descriptions

### Security
- Added input validation for RTMP error responses

---

## Versioning Scheme

This fork uses the following versioning scheme:
- Base version: Matches upstream OBS Studio version (e.g., 32.0.4)
- Suffix: `-vantis.X` where X is the fork-specific patch number

## Contributing

See [CONTRIBUTING.rst](CONTRIBUTING.rst) for guidelines on contributing to this project.

## Links

- Repository: https://github.com/vantisCorp/OBS---Test-unofficial
- Upstream: https://github.com/obsproject/obs-studio