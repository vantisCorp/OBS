# CI/CD Requirements for vantisCorp/OBS---Test-unofficial

## Current Status

This repository is **public** and owned by a **user account** (vantisCorp).
GitHub Actions is fully available for public repositories with unlimited minutes on standard runners.

## CI/CD Workflows

The following workflows are configured:

| Workflow | File | Trigger | Purpose |
|----------|------|---------|---------|
| Build Project | `build-project.yaml` | PR, push to master | Full multi-platform build (Linux, macOS, Windows, Flatpak) |
| Run Tests | `tests.yaml` | PR, push to master | Unit tests with CMocka |
| Sanitizers | `sanitizers.yaml` | PR, push to master | ASan, UBSan, TSan builds |
| Benchmarks | `benchmarks.yaml` | PR, push to master | Performance benchmark suite |
| CodeQL | `codeql.yaml` | PR, push to master, weekly | Security analysis (C++, Python) |
| CI Minimal | `ci-minimal.yaml` | PR, push to master | Quick status check |
| Pull (Format) | `pull.yaml` | PR | clang-format, gersemi, flatpak, Qt XML validation |
| Code Quality | `code-quality.yaml` | PR, push to master | Code quality checks |

## Required Dependencies (Linux)

The following system packages are required for building on Ubuntu:

### Core Build Tools
- `build-essential`, `cmake`, `extra-cmake-modules`, `pkg-config`, `clang`, `clang-format`

### Graphics & Display
- `libgl1-mesa-dev`, `libglu1-mesa-dev`, `libdrm-dev`
- `libx11-dev`, `libx11-xcb-dev`, `libxcb-dri3-dev`, `libxcb-randr0-dev`
- `libxcb-shm0-dev`, `libxcb-xfixes0-dev`, `libxcb-xinerama0-dev`
- `libxcb-composite0-dev`, `libxcb1-dev`, `libxcb-xinput-dev`
- `libxcomposite-dev`, `libxinerama-dev`, `libxrandr-dev`, `libxi-dev`
- `libxkbcommon-dev`, `libwayland-dev`

### Audio
- `libpulse-dev`, `libasound2-dev`, `libjack-dev`, `libspeexdsp-dev`

### Video/Media
- `libavcodec-dev`, `libavdevice-dev`, `libavfilter-dev`, `libavformat-dev`
- `libavutil-dev`, `libswresample-dev`, `libswscale-dev`
- `libv4l-dev`, `libvlc-dev`, `libx264-dev`, `libva-dev`

### Networking
- `libcurl4-openssl-dev`, `librist-dev`, `libsrt-openssl-dev`

### Other
- `libfontconfig1-dev`, `libfreetype6-dev`, `libluajit-5.1-dev`
- `libpci-dev`, `libpipewire-0.3-dev`, `libudev-dev`
- `libglib2.0-dev`, `libclang-dev`
- `libsimde-dev`, `uthash-dev`, `libjansson-dev`

## CMake Configuration Flags

For CI builds without UI/frontend:

```bash
cmake -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_UI=OFF \
  -DENABLE_FRONTEND=OFF \
  -DBUILD_TESTING=ON \
  -DENABLE_AJA=OFF \
  -DENABLE_DECKLINK=OFF \
  -DENABLE_NVENC=OFF \
  -DENABLE_QSV11=OFF \
  -DENABLE_VST=OFF \
  -DENABLE_WEBRTC=OFF
```

## Security Scanning

| Tool | Purpose | Configuration |
|------|---------|---------------|
| CodeQL | Static analysis for C++ and Python | `.github/workflows/codeql.yaml` |
| GitGuardian | Secret detection | External integration |
| Snyk | Dependency vulnerability scanning | External integration |
| Dependabot | Automated dependency updates | `.github/dependabot.yml` |

## Branch Protection (Recommended)

The `master` branch should have the following protections enabled:

- Require pull request reviews before merging
- Require status checks to pass (CI Minimal, Tests, Sanitizers)
- Require branches to be up to date before merging
- Restrict who can push to matching branches

## Versioning

This fork uses: `{upstream_version}-vantis.{patch_number}`
- Example: `32.0.4-vantis.11`
- See `CHANGELOG_VANTIS.md` for release history