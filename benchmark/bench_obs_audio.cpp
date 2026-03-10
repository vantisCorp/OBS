/*
 * Performance Benchmarks for Audio Processing
 * vantisCorp fork - Phase 3 Performance Improvements
 *
 * Measures performance of audio-related operations
 */

#include <benchmark/benchmark.h>

#include <obs-module.h>
#include <media-io/audio-io.h>

// =============================================================================
// Audio format operations
// =============================================================================

static void BM_audio_format_bytes_per_frame(benchmark::State &state)
{
	enum audio_format format = AUDIO_FORMAT_FLOAT;
	uint8_t channels = 2;

	for (auto _ : state) {
		uint8_t bytes = get_audio_bytes_per_channel(format);
		uint8_t planar_channels = (channels == 0) ? 0 : channels;
		size_t frame_size = bytes * planar_channels;

		benchmark::DoNotOptimize(frame_size);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_audio_format_bytes_per_frame);

// =============================================================================
// Sample rate operations
// =============================================================================

static void BM_audio_sample_rate_calc(benchmark::State &state)
{
	uint32_t sample_rate = 48000;
	uint64_t duration_ms = 1000;

	for (auto _ : state) {
		uint64_t samples = (sample_rate * duration_ms) / 1000;
		benchmark::DoNotOptimize(samples);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_audio_sample_rate_calc);

// =============================================================================
// Audio conversion simulation
// =============================================================================

static void BM_audio_float_to_int16(benchmark::State &state)
{
	const size_t samples = 1024;
	float src[samples];
	int16_t dst[samples];

	// Initialize source
	for (size_t i = 0; i < samples; i++) {
		src[i] = 0.5f * sinf(i * 0.01f);
	}

	for (auto _ : state) {
		for (size_t i = 0; i < samples; i++) {
			float sample = src[i];
			sample = (sample > 1.0f) ? 1.0f : (sample < -1.0f) ? -1.0f : sample;
			dst[i] = static_cast<int16_t>(sample * 32767.0f);
		}
		benchmark::ClobberMemory();
	}
	state.SetItemsProcessed(state.iterations() * samples);
}
BENCHMARK(BM_audio_float_to_int16);

// =============================================================================
// Audio mixing simulation
// =============================================================================

static void BM_audio_mix_stereo(benchmark::State &state)
{
	const size_t frames = 512;
	float left[frames];
	float right[frames];
	float mixed[frames];

	// Initialize
	for (size_t i = 0; i < frames; i++) {
		left[i] = 0.5f;
		right[i] = 0.5f;
	}

	for (auto _ : state) {
		for (size_t i = 0; i < frames; i++) {
			mixed[i] = (left[i] + right[i]) * 0.5f;
		}
		benchmark::ClobberMemory();
	}
	state.SetItemsProcessed(state.iterations() * frames);
}
BENCHMARK(BM_audio_mix_stereo);

// =============================================================================
// Volume operations
// =============================================================================

static void BM_audio_volume_apply(benchmark::State &state)
{
	const size_t frames = 1024;
	float samples[frames];
	float volume = 0.8f;

	// Initialize
	for (size_t i = 0; i < frames; i++) {
		samples[i] = 0.5f * sinf(i * 0.02f);
	}

	for (auto _ : state) {
		for (size_t i = 0; i < frames; i++) {
			samples[i] *= volume;
		}
		benchmark::ClobberMemory();
	}
	state.SetItemsProcessed(state.iterations() * frames);
}
BENCHMARK(BM_audio_volume_apply);

// =============================================================================
// Audio buffer operations
// =============================================================================

static void BM_audio_ring_buffer(benchmark::State &state)
{
	const size_t buffer_size = 4096;
	float buffer[buffer_size];
	size_t write_pos = 0;
	size_t read_pos = 0;

	for (size_t i = 0; i < buffer_size; i++) {
		buffer[i] = 0.0f;
	}

	size_t iterations = 0;
	for (auto _ : state) {
		// Write
		buffer[write_pos] = 0.5f;
		write_pos = (write_pos + 1) % buffer_size;

		// Read
		float value = buffer[read_pos];
		benchmark::DoNotOptimize(value);
		read_pos = (read_pos + 1) % buffer_size;

		iterations++;
	}
	state.SetItemsProcessed(iterations);
}
BENCHMARK(BM_audio_ring_buffer);

// =============================================================================
// Decibel conversions
// =============================================================================

static void BM_db_to_linear(benchmark::State &state)
{
	float db = -6.0f;

	for (auto _ : state) {
		float linear = powf(10.0f, db / 20.0f);
		benchmark::DoNotOptimize(linear);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_db_to_linear);

static void BM_linear_to_db(benchmark::State &state)
{
	float linear = 0.5f;

	for (auto _ : state) {
		float db = 20.0f * log10f(linear);
		benchmark::DoNotOptimize(db);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_linear_to_db);

// =============================================================================
// Speaker layout operations
// =============================================================================

static void BM_speaker_count(benchmark::State &state)
{
	enum speaker_layout speakers = SPEAKERS_STEREO;

	for (auto _ : state) {
		uint8_t count;
		switch (speakers) {
		case SPEAKERS_MONO:
			count = 1;
			break;
		case SPEAKERS_STEREO:
			count = 2;
			break;
		case SPEAKERS_2POINT1:
			count = 3;
			break;
		case SPEAKERS_4POINT0:
			count = 4;
			break;
		case SPEAKERS_4POINT1:
			count = 5;
			break;
		case SPEAKERS_5POINT1:
			count = 6;
			break;
		case SPEAKERS_7POINT1:
			count = 8;
			break;
		default:
			count = 2;
		}
		benchmark::DoNotOptimize(count);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_speaker_count);

BENCHMARK_MAIN();
