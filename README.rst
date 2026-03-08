OBS Studio - vantisCorp Fork <https://github.com/vantisCorp/OBS>
================================================================

.. image:: https://github.com/vantisCorp/OBS/actions/workflows/tests.yaml/badge.svg?branch=master
   :alt: vantisCorp/OBS Build Status - GitHub Actions
   :target: https://github.com/vantisCorp/OBS/actions/workflows/tests.yaml?query=branch%3Amaster

About This Fork
---------------

This is a performance-optimized fork of OBS Studio maintained by vantisCorp.
It includes various performance improvements and optimizations while maintaining
compatibility with the upstream project.

Key Improvements
----------------

- **SIMD Audio Mixing**: 4x-8x faster audio mixing using SSE/AVX/NEON
- **Lock-Free SPSC Queue**: Low-latency audio pipeline with atomic operations
- **Source Render Cache**: Optimized hot path rendering with cached properties
- **Memory Pool**: Pre-allocated memory for hot paths
- **Benchmark Suite**: 42 benchmarks for performance monitoring

What is OBS Studio?
-------------------

OBS Studio is software designed for capturing, compositing, encoding,
recording, and streaming video content, efficiently.

It's distributed under the GNU General Public License v2 (or any later
version) - see the accompanying COPYING file for more details.

Quick Links
-----------

- Upstream Repository: https://github.com/obsproject/obs-studio

- Website: https://obsproject.com

- Help/Documentation/Guides: https://github.com/obsproject/obs-studio/wiki

- Build Instructions: https://github.com/obsproject/obs-studio/wiki/Install-Instructions

- Developer/API Documentation: https://obsproject.com/docs

Changelog
---------

See CHANGELOG_VANTIS.md for the list of changes in this fork.

Building
--------

Build instructions are the same as upstream OBS Studio.
See: https://github.com/obsproject/obs-studio/wiki/Install-Instructions

License
-------

GNU General Public License v2 (or any later version).
See COPYING file for details.
