# Changelog - vantisCorp/OBS

All notable changes to this fork will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
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

## [32.0.4-vantis.1] - 2026-03-07

### Added
- Initial fork from obsproject/obs-studio
- Repository analysis and improvement recommendations

---

## Versioning Scheme

This fork uses the following versioning scheme:
- Base version: Matches upstream OBS Studio version (e.g., 32.0.4)
- Suffix: `-vantis.X` where X is the fork-specific patch number

## Contributing

See [CONTRIBUTING.rst](CONTRIBUTING.rst) for guidelines on contributing to this project.