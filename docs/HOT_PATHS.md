# OBS Studio Hot Paths Analysis

This document identifies performance-critical code paths in OBS Studio that should be optimized for maximum performance.

## Critical Hot Paths

### 1. Video Rendering Pipeline

**Location**: `libobs/obs-source.c` - `obs_source_video_render()`

**Called**: Every frame (60+ times per second)

**Operations**:
- Source activation checks
- Filter application
- Async texture processing
- Rendering callbacks

**Optimization Opportunities**:
- Cache frequently accessed properties
- Reduce lock contention in async rendering
- Consider SIMD for texture operations

```c
// Hot path - called every frame
void obs_source_video_render(obs_source_t *source)
{
    // This function processes every visible source every frame
    // Performance is critical here
}
```

---

### 2. Audio Processing Pipeline

**Location**: `libobs/obs-audio.c`

**Called**: Every audio chunk (thousands per second)

**Operations**:
- Audio buffering
- Sample rate conversion
- Mixing multiple sources
- Timestamp synchronization

**Optimization Opportunities**:
- Pre-allocate audio buffers
- SIMD for mixing operations
- Lock-free queue for audio chunks

```c
// Hot path - audio processing
// Called for every audio callback
void process_audio_sources(obs_source_t *source, ...)
{
    // Process audio for all active sources
}
```

---

### 3. Scene Item Management

**Location**: `libobs/obs-scene.c`

**Called**: Scene changes, item updates

**Operations**:
- Item z-order management
- Transform calculations
- Scene hierarchy traversal

**Optimization Opportunities**:
- Spatial indexing for item queries
- Cache transform matrices
- Batch updates

---

### 4. Frame Output Encoding

**Location**: `plugins/obs-ffmpeg/` and `plugins/obs-outputs/`

**Called**: Every encoded frame

**Operations**:
- Frame format conversion
- Encoder input preparation
- Packet handling

**Optimization Opportunities**:
- GPU-accelerated format conversion
- Zero-copy frame passing
- Thread pool for parallel encoding

---

### 5. Graphics Backend Operations

**Location**: `libobs/graphics/`

**Called**: Every draw call

**Operations**:
- Shader compilation
- State management
- Draw call batching

**Optimization Opportunities**:
- Shader caching
- State sorting
- Instanced rendering

---

## Performance Metrics to Monitor

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| Frame Time | < 16.6ms (60fps) | Profiler |
| Audio Latency | < 10ms | Audio callback timing |
| Memory Allocation | 0 in hot paths | Custom allocator tracking |
| Lock Contention | < 1% | Thread profiler |

---

## Benchmarking Guidelines

### Running Benchmarks

```bash
# Build with benchmarks enabled
cmake -B build -DENABLE_BENCHMARKS=ON
cmake --build build

# Run all benchmarks
./build/benchmark/bench_obs_data
./build/benchmark/bench_obs_source
./build/benchmark/bench_obs_audio
./build/benchmark/bench_obs_video
```

### Adding New Benchmarks

1. Create benchmark file in `benchmark/` directory
2. Follow Google Benchmark conventions
3. Add to `benchmark/CMakeLists.txt`
4. Document expected performance characteristics

---

## Optimization Checklist

- [ ] Profile before optimizing
- [ ] Establish baseline benchmarks
- [ ] Measure after each optimization
- [ ] Document performance gains
- [ ] Consider algorithmic improvements before micro-optimizations
- [ ] Test on multiple platforms

---

## Memory Management Hot Paths

### Critical Allocations to Avoid

| Function | Context | Alternative |
|----------|---------|-------------|
| `bmalloc()` | Per-frame | Pre-allocate pool |
| `bzalloc()` | Initialization | Acceptable |
| `bstrdup()` | Per-frame | Cache strings |

### Lock-Free Opportunities

| Data Structure | Current | Proposed |
|---------------|---------|----------|
| Audio buffer queue | Mutex | Lock-free SPSC queue |
| Frame queue | Mutex | Lock-free MPMC queue |
| Event system | Mutex | Atomic flags |

---

## Profiling Commands

### Linux (perf)

```bash
# Record performance data
perf record -g ./obs

# Analyze
perf report
perf annotate
```

### macOS (Instruments)

```bash
# Time Profiler
instruments -t "Time Profiler" ./obs

# Allocations
instruments -t "Allocations" ./obs
```

### Windows (Visual Studio)

Use the built-in Performance Profiler:
- CPU Usage
- Memory Usage
- GPU Usage

---

*Document Version: 1.0*
*Last Updated: 2026-03-07*
*vantisCorp OBS Fork - Phase 3*