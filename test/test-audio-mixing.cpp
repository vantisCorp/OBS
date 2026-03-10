/******************************************************************************
    Copyright (C) 2026 vantisCorp

    Unit tests for SIMD audio mixing functions.
******************************************************************************/

#include <gtest/gtest.h>
#include <cmath>
#include <cstring>
#include <vector>
#include "util/audio-mixing.h"

// Test fixture for audio mixing tests
class AudioMixingTest : public ::testing::Test {
protected:
	static const size_t TEST_FRAMES = 1024;
	std::vector<float> dest;
	std::vector<float> src;
	std::vector<float> expected;

	void SetUp() override
	{
		dest.resize(TEST_FRAMES);
		src.resize(TEST_FRAMES);
		expected.resize(TEST_FRAMES);

		// Initialize with test data
		for (size_t i = 0; i < TEST_FRAMES; i++) {
			dest[i] = static_cast<float>(i % 100) / 100.0f;
			src[i] = static_cast<float>((i + 50) % 100) / 100.0f;
		}
	}
};

// Test SIMD availability check
TEST_F(AudioMixingTest, SimdAvailable)
{
	// Should return 0 or 1, not crash
	int available = audio_simd_available();
	EXPECT_TRUE(available == 0 || available == 1);

	// Second call should return same result (cached)
	EXPECT_EQ(available, audio_simd_available());
}

// Test basic mixing
TEST_F(AudioMixingTest, BasicMix)
{
	std::vector<float> dest_copy = dest;

	// Calculate expected result
	for (size_t i = 0; i < TEST_FRAMES; i++) {
		expected[i] = dest[i] + src[i];
	}

	// Perform SIMD mix
	audio_mix_float_simd(dest.data(), src.data(), TEST_FRAMES);

	// Verify result
	for (size_t i = 0; i < TEST_FRAMES; i++) {
		EXPECT_NEAR(expected[i], dest[i], 1e-6f) << "Mismatch at frame " << i;
	}
}

// Test mixing with volume
TEST_F(AudioMixingTest, MixWithVolume)
{
	float volume = 0.5f;

	// Calculate expected result
	for (size_t i = 0; i < TEST_FRAMES; i++) {
		expected[i] = dest[i] + src[i] * volume;
	}

	// Perform SIMD mix with volume
	audio_mix_float_volume_simd(dest.data(), src.data(), TEST_FRAMES, volume);

	// Verify result
	for (size_t i = 0; i < TEST_FRAMES; i++) {
		EXPECT_NEAR(expected[i], dest[i], 1e-6f) << "Mismatch at frame " << i;
	}
}

// Test mixing with zero volume (should be no-op)
TEST_F(AudioMixingTest, MixWithZeroVolume)
{
	std::vector<float> dest_copy = dest;

	// Mix with zero volume
	audio_mix_float_volume_simd(dest.data(), src.data(), TEST_FRAMES, 0.0f);

	// Destination should be unchanged
	for (size_t i = 0; i < TEST_FRAMES; i++) {
		EXPECT_FLOAT_EQ(dest_copy[i], dest[i]) << "Changed at frame " << i;
	}
}

// Test mixing with unit volume (should be same as basic mix)
TEST_F(AudioMixingTest, MixWithUnitVolume)
{
	std::vector<float> dest2 = dest;

	// Calculate expected with basic mix
	for (size_t i = 0; i < TEST_FRAMES; i++) {
		expected[i] = dest[i] + src[i];
	}

	// Perform mix with volume = 1.0
	audio_mix_float_volume_simd(dest2.data(), src.data(), TEST_FRAMES, 1.0f);

	// Verify result
	for (size_t i = 0; i < TEST_FRAMES; i++) {
		EXPECT_NEAR(expected[i], dest2[i], 1e-6f) << "Mismatch at frame " << i;
	}
}

// Test mixing with zero frames (should be no-op)
TEST_F(AudioMixingTest, MixZeroFrames)
{
	std::vector<float> dest_copy = dest;

	// Mix zero frames
	audio_mix_float_simd(dest.data(), src.data(), 0);

	// Destination should be unchanged
	for (size_t i = 0; i < TEST_FRAMES; i++) {
		EXPECT_FLOAT_EQ(dest_copy[i], dest[i]);
	}
}

// Test mixing odd number of frames (non-aligned)
TEST_F(AudioMixingTest, MixOddFrames)
{
	const size_t odd_frames = 17; // Not aligned to 4 or 8

	std::vector<float> small_dest(odd_frames);
	std::vector<float> small_src(odd_frames);

	for (size_t i = 0; i < odd_frames; i++) {
		small_dest[i] = static_cast<float>(i) * 0.1f;
		small_src[i] = static_cast<float>(i + 1) * 0.1f;
		expected[i] = small_dest[i] + small_src[i];
	}

	audio_mix_float_simd(small_dest.data(), small_src.data(), odd_frames);

	for (size_t i = 0; i < odd_frames; i++) {
		EXPECT_NEAR(expected[i], small_dest[i], 1e-6f) << "Mismatch at frame " << i;
	}
}

// Test large buffer mixing
TEST_F(AudioMixingTest, LargeBuffer)
{
	const size_t large_frames = 65536; // 64K frames
	std::vector<float> large_dest(large_frames);
	std::vector<float> large_src(large_frames);

	for (size_t i = 0; i < large_frames; i++) {
		large_dest[i] = 0.5f;
		large_src[i] = 0.25f;
	}

	audio_mix_float_simd(large_dest.data(), large_src.data(), large_frames);

	// All values should be 0.75f
	for (size_t i = 0; i < large_frames; i++) {
		EXPECT_NEAR(0.75f, large_dest[i], 1e-6f) << "Mismatch at frame " << i;
	}
}

// Test negative values
TEST_F(AudioMixingTest, NegativeValues)
{
	const size_t frames = 100;
	std::vector<float> neg_dest(frames);
	std::vector<float> neg_src(frames);

	for (size_t i = 0; i < frames; i++) {
		neg_dest[i] = -0.5f + (i % 10) * 0.1f;
		neg_src[i] = -0.3f + (i % 5) * 0.1f;
		expected[i] = neg_dest[i] + neg_src[i];
	}

	audio_mix_float_simd(neg_dest.data(), neg_src.data(), frames);

	for (size_t i = 0; i < frames; i++) {
		EXPECT_NEAR(expected[i], neg_dest[i], 1e-6f) << "Mismatch at frame " << i;
	}
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
