/*
 * Performance Benchmarks for Video Processing
 * vantisCorp fork - Phase 3 Performance Improvements
 *
 * Measures performance of video-related operations
 */

#include <benchmark/benchmark.h>

#include <obs-module.h>
#include <graphics/graphics.h>
#include <graphics/matrix4.h>

// =============================================================================
// Video format operations
// =============================================================================

static void BM_video_format_bpp(benchmark::State &state)
{
	enum video_format format = VIDEO_FORMAT_NV12;

	for (auto _ : state) {
		uint8_t bpp;
		switch (format) {
		case VIDEO_FORMAT_I420:
		case VIDEO_FORMAT_NV12:
			bpp = 12;
			break;
		case VIDEO_FORMAT_YVYU:
		case VIDEO_FORMAT_YUY2:
		case VIDEO_FORMAT_UYVY:
		case VIDEO_FORMAT_RGBA:
		case VIDEO_FORMAT_BGRA:
		case VIDEO_FORMAT_BGRX:
			bpp = 32;
			break;
		case VIDEO_FORMAT_I444:
			bpp = 24;
			break;
		default:
			bpp = 12;
		}
		benchmark::DoNotOptimize(bpp);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_video_format_bpp);

// =============================================================================
// Frame size calculations
// =============================================================================

static void BM_video_frame_size_nv12(benchmark::State &state)
{
	uint32_t width = 1920;
	uint32_t height = 1080;

	for (auto _ : state) {
		size_t y_size = width * height;
		size_t uv_size = (width / 2) * (height / 2) * 2;
		size_t total = y_size + uv_size;

		benchmark::DoNotOptimize(total);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_video_frame_size_nv12);

static void BM_video_frame_size_rgba(benchmark::State &state)
{
	uint32_t width = 1920;
	uint32_t height = 1080;

	for (auto _ : state) {
		size_t total = width * height * 4;
		benchmark::DoNotOptimize(total);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_video_frame_size_rgba);

// =============================================================================
// YUV to RGB conversion simulation
// =============================================================================

static void BM_yuv_to_rgb_pixel(benchmark::State &state)
{
	uint8_t y = 128;
	uint8_t u = 128;
	uint8_t v = 128;

	for (auto _ : state) {
		int r = y + (int)(1.402f * (v - 128));
		int g = y - (int)(0.344f * (u - 128)) - (int)(0.714f * (v - 128));
		int b = y + (int)(1.772f * (u - 128));

		// Clamp
		r = (r < 0) ? 0 : (r > 255) ? 255 : r;
		g = (g < 0) ? 0 : (g > 255) ? 255 : g;
		b = (b < 0) ? 0 : (b > 255) ? 255 : b;

		benchmark::DoNotOptimize(r);
		benchmark::DoNotOptimize(g);
		benchmark::DoNotOptimize(b);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_yuv_to_rgb_pixel);

// =============================================================================
// Matrix operations for transforms
// =============================================================================

static void BM_matrix4_identity(benchmark::State &state)
{
	struct matrix4 mat;

	for (auto _ : state) {
		mat.x.x = 1.0f;
		mat.x.y = 0.0f;
		mat.x.z = 0.0f;
		mat.x.w = 0.0f;
		mat.y.x = 0.0f;
		mat.y.y = 1.0f;
		mat.y.z = 0.0f;
		mat.y.w = 0.0f;
		mat.z.x = 0.0f;
		mat.z.y = 0.0f;
		mat.z.z = 1.0f;
		mat.z.w = 0.0f;
		mat.t.x = 0.0f;
		mat.t.y = 0.0f;
		mat.t.z = 0.0f;
		mat.t.w = 1.0f;

		benchmark::DoNotOptimize(mat);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_matrix4_identity);

static void BM_matrix4_mul_vec4(benchmark::State &state)
{
	struct matrix4 mat = {{1.0f, 0.0f, 0.0f, 0.0f},
			      {0.0f, 1.0f, 0.0f, 0.0f},
			      {0.0f, 0.0f, 1.0f, 0.0f},
			      {0.0f, 0.0f, 0.0f, 1.0f}};
	struct vec4 v = {100.0f, 200.0f, 0.0f, 1.0f};
	struct vec4 result;

	for (auto _ : state) {
		result.x = mat.x.x * v.x + mat.y.x * v.y + mat.z.x * v.z + mat.t.x * v.w;
		result.y = mat.x.y * v.x + mat.y.y * v.y + mat.z.y * v.z + mat.t.y * v.w;
		result.z = mat.x.z * v.x + mat.y.z * v.y + mat.z.z * v.z + mat.t.z * v.w;
		result.w = mat.x.w * v.x + mat.y.w * v.y + mat.z.w * v.z + mat.t.w * v.w;

		benchmark::DoNotOptimize(result);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_matrix4_mul_vec4);

// =============================================================================
// Color space conversions
// =============================================================================

static void BM_color_rgba_to_bgra(benchmark::State &state)
{
	const size_t pixels = 1920 * 1080;
	uint32_t src[pixels];
	uint32_t dst[pixels];

	for (size_t i = 0; i < pixels; i++) {
		src[i] = 0xFF4080C0;
	}

	for (auto _ : state) {
		for (size_t i = 0; i < pixels; i++) {
			uint8_t r = (src[i] >> 0) & 0xFF;
			uint8_t g = (src[i] >> 8) & 0xFF;
			uint8_t b = (src[i] >> 16) & 0xFF;
			uint8_t a = (src[i] >> 24) & 0xFF;
			dst[i] = (a << 24) | (r << 16) | (g << 8) | b;
		}
		benchmark::ClobberMemory();
	}
	state.SetItemsProcessed(state.iterations() * pixels);
}
BENCHMARK(BM_color_rgba_to_bgra);

// =============================================================================
// FPS calculations
// =============================================================================

static void BM_fps_calculation(benchmark::State &state)
{
	uint32_t fps_num = 60000;
	uint32_t fps_den = 1001;

	for (auto _ : state) {
		double fps = static_cast<double>(fps_num) / static_cast<double>(fps_den);
		double frame_time_ms = 1000.0 / fps;

		benchmark::DoNotOptimize(fps);
		benchmark::DoNotOptimize(frame_time_ms);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_fps_calculation);

// =============================================================================
// Resolution scaling
// =============================================================================

static void BM_scale_dimensions(benchmark::State &state)
{
	uint32_t src_width = 1920;
	uint32_t src_height = 1080;
	float scale = 0.5f;

	for (auto _ : state) {
		uint32_t dst_width = static_cast<uint32_t>(src_width * scale + 0.5f);
		uint32_t dst_height = static_cast<uint32_t>(src_height * scale + 0.5f);

		// Ensure even dimensions
		dst_width = (dst_width / 2) * 2;
		dst_height = (dst_height / 2) * 2;

		benchmark::DoNotOptimize(dst_width);
		benchmark::DoNotOptimize(dst_height);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_scale_dimensions);

// =============================================================================
// Aspect ratio calculations
// =============================================================================

static void BM_aspect_ratio(benchmark::State &state)
{
	uint32_t width = 1920;
	uint32_t height = 1080;

	for (auto _ : state) {
		float aspect = static_cast<float>(width) / static_cast<float>(height);

		// Common aspect ratios
		bool is_16_9 = (fabs(aspect - 16.0f / 9.0f) < 0.01f);
		bool is_4_3 = (fabs(aspect - 4.0f / 3.0f) < 0.01f);
		bool is_21_9 = (fabs(aspect - 21.0f / 9.0f) < 0.01f);

		benchmark::DoNotOptimize(aspect);
		benchmark::DoNotOptimize(is_16_9);
		benchmark::DoNotOptimize(is_4_3);
		benchmark::DoNotOptimize(is_21_9);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_aspect_ratio);

BENCHMARK_MAIN();
