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

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Mix audio samples from source to destination using SIMD optimization.
 * 
 * @param dest Destination buffer (will be modified: dest[i] += src[i])
 * @param src Source buffer
 * @param frames Number of audio frames to mix
 * 
 * This function uses SIMD instructions (SSE/AVX) when available,
 * falling back to scalar implementation otherwise.
 */
void audio_mix_float_simd(float *dest, const float *src, size_t frames);

/**
 * Mix audio samples with volume control using SIMD optimization.
 * 
 * @param dest Destination buffer
 * @param src Source buffer
 * @param frames Number of audio frames
 * @param volume Volume multiplier (0.0 to 1.0)
 */
void audio_mix_float_volume_simd(float *dest, const float *src, size_t frames, float volume);

/**
 * Check if SIMD is available on the current CPU.
 * 
 * @return 1 if SIMD is available, 0 otherwise
 */
int audio_simd_available(void);

#ifdef __cplusplus
}
#endif
