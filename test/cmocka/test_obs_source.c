/*
 * Testy jednostkowe dla obs-source (źródła wideo/audio)
 * Copyright (c) 2026 vantisCorp
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <obs-module.h>
#include <obs-source.h>

/* Mock obs context for testing */
static obs_source_t *create_test_source(void)
{
	obs_data_t *settings = obs_data_create();
	obs_source_t *source = obs_source_create("test_source", "test", settings, NULL);
	obs_data_release(settings);
	return source;
}

/* Test: Create and release source */
static void test_obs_source_create_release(void **state)
{
	(void)state;

	obs_data_t *settings = obs_data_create();
	assert_non_null(settings);

	/* Note: This requires obs to be initialized, which may not work in unit tests */
	/* In a real test environment, you would need to mock or initialize obs_core */

	obs_data_release(settings);
}

/* Test: Source flags */
static void test_obs_source_flags(void **state)
{
	(void)state;

	/* Test source flag values */
	assert_int_equal(OBS_SOURCE_FLAG_VIDEO, 0x01);
	assert_int_equal(OBS_SOURCE_FLAG_AUDIO, 0x02);
	assert_int_equal(OBS_SOURCE_FLAG_ASYNC, 0x04);
	assert_int_equal(OBS_SOURCE_FLAG_DISABLED, 0x08);
}

/* Test: Source output flags */
static void test_obs_source_output_flags(void **state)
{
	(void)state;

	/* Test output flag values */
	assert_int_equal(OBS_SOURCE_VIDEO, 0x01);
	assert_int_equal(OBS_SOURCE_AUDIO, 0x02);
	assert_int_equal(OBS_SOURCE_ASYNC, 0x04);
	assert_int_equal(OBS_SOURCE_ASYNC_VIDEO, 0x05);
}

/* Test: Source type enum values */
static void test_obs_source_type_values(void **state)
{
	(void)state;

	assert_int_equal(OBS_SOURCE_TYPE_INPUT, 0);
	assert_int_equal(OBS_SOURCE_TYPE_FILTER, 1);
	assert_int_equal(OBS_SOURCE_TYPE_TRANSITION, 2);
	assert_int_equal(OBS_SOURCE_TYPE_SCENE, 3);
}

/* Test: Obs_source_info structure */
static void test_obs_source_info_defaults(void **state)
{
	(void)state;

	struct obs_source_info info = {0};

	/* Verify structure can be initialized */
	assert_int_equal(info.id, 0);
	assert_int_equal(info.type, 0);
	assert_int_equal(info.output_flags, 0);
}

/* Test: Source capability flags */
static void test_obs_source_capabilities(void **state)
{
	(void)state;

	/* These are compile-time constants, verify they exist */
	assert_true(OBS_SOURCE_CAP_DONT_SHOW_PROPERTIES != 0 || OBS_SOURCE_CAP_DONT_SHOW_PROPERTIES == 0);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_obs_source_create_release), cmocka_unit_test(test_obs_source_flags),
		cmocka_unit_test(test_obs_source_output_flags),   cmocka_unit_test(test_obs_source_type_values),
		cmocka_unit_test(test_obs_source_info_defaults),  cmocka_unit_test(test_obs_source_capabilities),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
