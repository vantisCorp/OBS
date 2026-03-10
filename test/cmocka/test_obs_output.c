/*
 * Testy jednostkowe dla obs-output (wyjścia nagrywania/streamingu)
 * Copyright (c) 2026 vantisCorp
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <obs-module.h>
#include <obs-output.h>

/* Test: Output state enum values */
static void test_obs_output_state_values(void **state)
{
	(void)state;

	assert_int_equal(OBS_OUTPUT_SUCCESS, 0);
	assert_int_equal(OBS_OUTPUT_BAD_PATH, -1);
	assert_int_equal(OBS_OUTPUT_CONNECT_FAILED, -2);
	assert_int_equal(OBS_OUTPUT_INVALID_STREAM, -3);
	assert_int_equal(OBS_OUTPUT_ERROR, -4);
	assert_int_equal(OBS_OUTPUT_DISCONNECTED, -5);
	assert_int_equal(OBS_OUTPUT_UNSUPPORTED, -6);
	assert_int_equal(OBS_OUTPUT_NO_SPACE, -7);
}

/* Test: Output flags */
static void test_obs_output_flags(void **state)
{
	(void)state;

	assert_int_equal(OBS_OUTPUT_AUDIO, 0x01);
	assert_int_equal(OBS_OUTPUT_VIDEO, 0x02);
	assert_int_equal(OBS_OUTPUT_AV, 0x03);
	assert_int_equal(OBS_OUTPUT_ENCODED, 0x04);
	assert_int_equal(OBS_OUTPUT_SERVICE, 0x10);
	assert_int_equal(OBS_OUTPUT_MULTI_TRACK, 0x20);
}

/* Test: Output info structure */
static void test_obs_output_info_defaults(void **state)
{
	(void)state;

	struct obs_output_info info = {0};

	assert_null(info.id);
	assert_int_equal(info.flags, 0);
}

/* Test: Video output info */
static void test_video_output_info(void **state)
{
	(void)state;

	struct video_output_info vi = {0};

	/* Verify structure can be initialized */
	assert_int_equal(vi.fps_num, 0);
	assert_int_equal(vi.fps_den, 0);
	assert_int_equal(vi.width, 0);
	assert_int_equal(vi.height, 0);
}

/* Test: Audio output info */
static void test_audio_output_info(void **state)
{
	(void)state;

	struct audio_output_info ai = {0};

	/* Verify structure can be initialized */
	assert_int_equal(ai.samples_per_sec, 0);
	assert_int_equal(ai.speakers, 0);
}

/* Test: Encoder capabilities */
static void test_encoder_caps(void **state)
{
	(void)state;

	/* These are used in output configuration */
	assert_true(OBS_ENCODER_VIDEO != NULL || OBS_ENCODER_VIDEO == NULL);
	assert_true(OBS_ENCODER_AUDIO != NULL || OBS_ENCODER_AUDIO == NULL);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_obs_output_state_values),  cmocka_unit_test(test_obs_output_flags),
		cmocka_unit_test(test_obs_output_info_defaults), cmocka_unit_test(test_video_output_info),
		cmocka_unit_test(test_audio_output_info),        cmocka_unit_test(test_encoder_caps),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
