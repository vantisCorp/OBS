/******************************************************************************
    Copyright (C) 2026 vantisCorp

    SIMD-optimized audio mixing functions for OBS Studio.
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "audio-mixing.h"
#include <string.h>

/* Check for SIMD availability */
#if defined(__SSE__) || defined(_M_X64) || defined(_M_AMD64)
#define HAVE_SSE 1
#include <xmmintrin.h>
#if defined(__SSE2__) || defined(_M_X64)
#define HAVE_SSE2 1
#include <emmintrin.h>
#endif
#if defined(__SSE3__)
#define HAVE_SSE3 1
#include <pmmintrin.h>
#endif
#if defined(__AVX__)
#define HAVE_AVX 1
#include <immintrin.h>
#endif
#endif

/* ARM NEON support */
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#define HAVE_NEON 1
#include <arm_neon.h>
#endif

static int simd_available = -1; /* -1 = not checked yet */

int audio_simd_available(void)
{
	if (simd_available >= 0)
		return simd_available;

	simd_available = 0;

#if defined(HAVE_SSE)
	/* SSE is available at compile time */
	simd_available = 1;
#elif defined(HAVE_NEON)
	/* NEON is available at compile time */
	simd_available = 1;
#endif

	return simd_available;
}

/* Scalar fallback implementation */
static inline void audio_mix_float_scalar(float *dest, const float *src, size_t frames)
{
	for (size_t i = 0; i < frames; i++) {
		dest[i] += src[i];
	}
}

/* Scalar fallback with volume */
static inline void audio_mix_float_volume_scalar(float *dest, const float *src, size_t frames, float volume)
{
	for (size_t i = 0; i < frames; i++) {
		dest[i] += src[i] * volume;
	}
}

#if defined(HAVE_AVX)
/* AVX implementation - processes 8 floats at a time */
static inline void audio_mix_float_avx(float *dest, const float *src, size_t frames)
{
	size_t i = 0;
	const size_t vec_size = 8;
	size_t aligned_frames = frames - (frames % vec_size);

	/* Process 8 floats at a time using AVX */
	for (; i < aligned_frames; i += vec_size) {
		__m256 d = _mm256_loadu_ps(&dest[i]);
		__m256 s = _mm256_loadu_ps(&src[i]);
		_mm256_storeu_ps(&dest[i], _mm256_add_ps(d, s));
	}

	/* Handle remaining samples */
	for (; i < frames; i++) {
		dest[i] += src[i];
	}
}

static inline void audio_mix_float_volume_avx(float *dest, const float *src, size_t frames, float volume)
{
	size_t i = 0;
	const size_t vec_size = 8;
	size_t aligned_frames = frames - (frames % vec_size);
	__m256 vol = _mm256_set1_ps(volume);

	for (; i < aligned_frames; i += vec_size) {
		__m256 d = _mm256_loadu_ps(&dest[i]);
		__m256 s = _mm256_loadu_ps(&src[i]);
		s = _mm256_mul_ps(s, vol);
		_mm256_storeu_ps(&dest[i], _mm256_add_ps(d, s));
	}

	for (; i < frames; i++) {
		dest[i] += src[i] * volume;
	}
}
#endif /* HAVE_AVX */

#if defined(HAVE_SSE)
/* SSE implementation - processes 4 floats at a time */
static inline void audio_mix_float_sse(float *dest, const float *src, size_t frames)
{
	size_t i = 0;
	const size_t vec_size = 4;
	size_t aligned_frames = frames - (frames % vec_size);

	/* Process 4 floats at a time using SSE */
	for (; i < aligned_frames; i += vec_size) {
		__m128 d = _mm_loadu_ps(&dest[i]);
		__m128 s = _mm_loadu_ps(&src[i]);
		_mm_storeu_ps(&dest[i], _mm_add_ps(d, s));
	}

	/* Handle remaining samples */
	for (; i < frames; i++) {
		dest[i] += src[i];
	}
}

static inline void audio_mix_float_volume_sse(float *dest, const float *src, size_t frames, float volume)
{
	size_t i = 0;
	const size_t vec_size = 4;
	size_t aligned_frames = frames - (frames % vec_size);
	__m128 vol = _mm_set1_ps(volume);

	for (; i < aligned_frames; i += vec_size) {
		__m128 d = _mm_loadu_ps(&dest[i]);
		__m128 s = _mm_loadu_ps(&src[i]);
		s = _mm_mul_ps(s, vol);
		_mm_storeu_ps(&dest[i], _mm_add_ps(d, s));
	}

	for (; i < frames; i++) {
		dest[i] += src[i] * volume;
	}
}
#endif /* HAVE_SSE */

#if defined(HAVE_NEON)
/* NEON implementation - processes 4 floats at a time */
static inline void audio_mix_float_neon(float *dest, const float *src, size_t frames)
{
	size_t i = 0;
	const size_t vec_size = 4;
	size_t aligned_frames = frames - (frames % vec_size);

	for (; i < aligned_frames; i += vec_size) {
		float32x4_t d = vld1q_f32(&dest[i]);
		float32x4_t s = vld1q_f32(&src[i]);
		vst1q_f32(&dest[i], vaddq_f32(d, s));
	}

	for (; i < frames; i++) {
		dest[i] += src[i];
	}
}

static inline void audio_mix_float_volume_neon(float *dest, const float *src, size_t frames, float volume)
{
	size_t i = 0;
	const size_t vec_size = 4;
	size_t aligned_frames = frames - (frames % vec_size);
	float32x4_t vol = vdupq_n_f32(volume);

	for (; i < aligned_frames; i += vec_size) {
		float32x4_t d = vld1q_f32(&dest[i]);
		float32x4_t s = vld1q_f32(&src[i]);
		s = vmulq_f32(s, vol);
		vst1q_f32(&dest[i], vaddq_f32(d, s));
	}

	for (; i < frames; i++) {
		dest[i] += src[i] * volume;
	}
}
#endif /* HAVE_NEON */

/* Public API - dispatches to best available implementation */
void audio_mix_float_simd(float *dest, const float *src, size_t frames)
{
	if (frames == 0)
		return;

#if defined(HAVE_AVX)
	audio_mix_float_avx(dest, src, frames);
#elif defined(HAVE_SSE)
	audio_mix_float_sse(dest, src, frames);
#elif defined(HAVE_NEON)
	audio_mix_float_neon(dest, src, frames);
#else
	audio_mix_float_scalar(dest, src, frames);
#endif
}

void audio_mix_float_volume_simd(float *dest, const float *src, size_t frames, float volume)
{
	if (frames == 0 || volume == 0.0f)
		return;

	if (volume == 1.0f) {
		audio_mix_float_simd(dest, src, frames);
		return;
	}

#if defined(HAVE_AVX)
	audio_mix_float_volume_avx(dest, src, frames, volume);
#elif defined(HAVE_SSE)
	audio_mix_float_volume_sse(dest, src, frames, volume);
#elif defined(HAVE_NEON)
	audio_mix_float_volume_neon(dest, src, frames, volume);
#else
	audio_mix_float_volume_scalar(dest, src, frames, volume);
#endif
}
