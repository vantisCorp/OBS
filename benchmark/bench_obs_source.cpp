/*
 * Performance Benchmarks for obs_source
 * vantisCorp fork - Phase 3 Performance Improvements
 *
 * Measures performance of source-related operations
 */

#include <benchmark/benchmark.h>

#include <obs-module.h>
#include <obs-source.h>

// =============================================================================
// Source info structure benchmarks
// =============================================================================

static void BM_obs_source_info_init(benchmark::State& state) {
    for (auto _ : state) {
        struct obs_source_info info = {0};
        info.id = "benchmark_source";
        info.type = OBS_SOURCE_TYPE_INPUT;
        info.output_flags = OBS_SOURCE_VIDEO;

        benchmark::DoNotOptimize(info);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_source_info_init);

// =============================================================================
// Source flags operations
// =============================================================================

static void BM_obs_source_flag_checks(benchmark::State& state) {
    uint32_t flags = OBS_SOURCE_VIDEO | OBS_SOURCE_AUDIO | OBS_SOURCE_ASYNC;

    for (auto _ : state) {
        bool has_video = (flags & OBS_SOURCE_VIDEO) != 0;
        bool has_audio = (flags & OBS_SOURCE_AUDIO) != 0;
        bool is_async = (flags & OBS_SOURCE_ASYNC) != 0;

        benchmark::DoNotOptimize(has_video);
        benchmark::DoNotOptimize(has_audio);
        benchmark::DoNotOptimize(is_async);
    }
    state.SetItemsProcessed(state.iterations() * 3);
}
BENCHMARK(BM_obs_source_flag_checks);

// =============================================================================
// Source type operations
// =============================================================================

static void BM_obs_source_type_checks(benchmark::State& state) {
    enum obs_source_type type = OBS_SOURCE_TYPE_INPUT;

    for (auto _ : state) {
        const char *type_str = "unknown";
        switch (type) {
        case OBS_SOURCE_TYPE_INPUT:
            type_str = "input";
            break;
        case OBS_SOURCE_TYPE_FILTER:
            type_str = "filter";
            break;
        case OBS_SOURCE_TYPE_TRANSITION:
            type_str = "transition";
            break;
        case OBS_SOURCE_TYPE_SCENE:
            type_str = "scene";
            break;
        }
        benchmark::DoNotOptimize(type_str);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_source_type_checks);

// =============================================================================
// Video frame operations (mock)
// =============================================================================

struct mock_video_frame {
    uint8_t *data[4];
    uint32_t linesize[4];
    uint64_t timestamp;
};

static void BM_video_frame_init(benchmark::State& state) {
    for (auto _ : state) {
        struct mock_video_frame frame = {0};
        frame.timestamp = 0;
        for (int i = 0; i < 4; i++) {
            frame.data[i] = nullptr;
            frame.linesize[i] = 0;
        }
        benchmark::DoNotOptimize(frame);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_video_frame_init);

// =============================================================================
// Audio data operations (mock)
// =============================================================================

struct mock_audio_data {
    uint8_t *data[8];
    uint32_t frames;
    uint64_t timestamp;
};

static void BM_audio_data_init(benchmark::State& state) {
    for (auto _ : state) {
        struct mock_audio_data audio = {0};
        audio.frames = 0;
        audio.timestamp = 0;
        for (int i = 0; i < 8; i++) {
            audio.data[i] = nullptr;
        }
        benchmark::DoNotOptimize(audio);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_audio_data_init);

// =============================================================================
// Source property operations
// =============================================================================

static void BM_source_property_string(benchmark::State& state) {
    const char *prop_name = "test_property";
    const char *prop_value = "test_value_1234567890";

    for (auto _ : state) {
        size_t len = strlen(prop_value);
        benchmark::DoNotOptimize(len);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_source_property_string);

// =============================================================================
// Rectangle operations
// =============================================================================

static void BM_obs_source_rect_calc(benchmark::State& state) {
    struct vec2 pos = {100.0f, 100.0f};
    struct vec2 scale = {1.0f, 1.0f};
    float rotation = 0.0f;
    uint32_t width = 1920;
    uint32_t height = 1080;

    for (auto _ : state) {
        // Simulate transform calculation
        float cos_r = cosf(rotation);
        float sin_r = sinf(rotation);

        float x1 = pos.x;
        float y1 = pos.y;
        float x2 = pos.x + width * scale.x * cos_r;
        float y2 = pos.y + width * scale.x * sin_r;

        benchmark::DoNotOptimize(x1);
        benchmark::DoNotOptimize(y1);
        benchmark::DoNotOptimize(x2);
        benchmark::DoNotOptimize(y2);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_source_rect_calc);

// =============================================================================
// Hotkey operations
// =============================================================================

static void BM_hotkey_id_generation(benchmark::State& state) {
    int counter = 0;
    for (auto _ : state) {
        obs_hotkey_id id = static_cast<obs_hotkey_id>(counter++);
        benchmark::DoNotOptimize(id);
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_hotkey_id_generation);

BENCHMARK_MAIN();