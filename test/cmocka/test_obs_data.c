/*
 * Testy jednostkowe dla obs-data (serializacja danych)
 * Copyright (c) 2026 vantisCorp
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <obs-module.h>
#include <obs-data.h>

/* Test: Tworzenie i niszczenie obs_data */
static void test_obs_data_create_destroy(void **state)
{
	(void)state;

	obs_data_t *data = obs_data_create();
	assert_non_null(data);

	obs_data_release(data);
}

/* Test: Ustawianie i pobieranie wartości string */
static void test_obs_data_set_get_string(void **state)
{
	(void)state;

	obs_data_t *data = obs_data_create();
	assert_non_null(data);

	obs_data_set_string(data, "test_key", "test_value");

	const char *value = obs_data_get_string(data, "test_key");
	assert_string_equal(value, "test_value");

	obs_data_release(data);
}

/* Test: Ustawianie i pobieranie wartości int */
static void test_obs_data_set_get_int(void **state)
{
	(void)state;

	obs_data_t *data = obs_data_create();
	assert_non_null(data);

	obs_data_set_int(data, "int_key", 42);

	long long value = obs_data_get_int(data, "int_key");
	assert_int_equal(value, 42);

	obs_data_release(data);
}

/* Test: Ustawianie i pobieranie wartości double */
static void test_obs_data_set_get_double(void **state)
{
	(void)state;

	obs_data_t *data = obs_data_create();
	assert_non_null(data);

	obs_data_set_double(data, "double_key", 3.14159);

	double value = obs_data_get_double(data, "double_key");
	assert_float_equal(value, 3.14159, 0.0001);

	obs_data_release(data);
}

/* Test: Ustawianie i pobieranie wartości bool */
static void test_obs_data_set_get_bool(void **state)
{
	(void)state;

	obs_data_t *data = obs_data_create();
	assert_non_null(data);

	obs_data_set_bool(data, "bool_key", true);

	bool value = obs_data_get_bool(data, "bool_key");
	assert_true(value);

	obs_data_set_bool(data, "bool_key", false);
	value = obs_data_get_bool(data, "bool_key");
	assert_false(value);

	obs_data_release(data);
}

/* Test: Zagnieżdżone obs_data */
static void test_obs_data_nested(void **state)
{
	(void)state;

	obs_data_t *parent = obs_data_create();
	obs_data_t *child = obs_data_create();

	assert_non_null(parent);
	assert_non_null(child);

	obs_data_set_string(child, "child_key", "child_value");
	obs_data_set_obj(parent, "nested", child);

	obs_data_t *retrieved = obs_data_get_obj(parent, "nested");
	assert_non_null(retrieved);

	const char *value = obs_data_get_string(retrieved, "child_key");
	assert_string_equal(value, "child_value");

	obs_data_release(child);
	obs_data_release(parent);
}

/* Test: obs_data_array */
static void test_obs_data_array(void **state)
{
	(void)state;

	obs_data_array_t *array = obs_data_array_create();
	assert_non_null(array);

	obs_data_t *item1 = obs_data_create();
	obs_data_t *item2 = obs_data_create();

	obs_data_set_string(item1, "name", "item1");
	obs_data_set_string(item2, "name", "item2");

	obs_data_array_push_back(array, item1);
	obs_data_array_push_back(array, item2);

	assert_int_equal(obs_data_array_count(array), 2);

	obs_data_t *retrieved = obs_data_array_item(array, 0);
	assert_non_null(retrieved);

	const char *name = obs_data_get_string(retrieved, "name");
	assert_string_equal(name, "item1");

	obs_data_release(retrieved);
	obs_data_release(item1);
	obs_data_release(item2);
	obs_data_array_release(array);
}

/* Test: JSON serialization */
static void test_obs_data_json(void **state)
{
	(void)state;

	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "name", "test");
	obs_data_set_int(data, "value", 123);
	obs_data_set_bool(data, "enabled", true);

	char *json = obs_data_get_json(data);
	assert_non_null(json);

	obs_data_t *parsed = obs_data_create_from_json(json);
	assert_non_null(parsed);

	assert_string_equal(obs_data_get_string(parsed, "name"), "test");
	assert_int_equal(obs_data_get_int(parsed, "value"), 123);
	assert_true(obs_data_get_bool(parsed, "enabled"));

	bfree(json);
	obs_data_release(parsed);
	obs_data_release(data);
}

/* Test: Default values */
static void test_obs_data_defaults(void **state)
{
	(void)state;

	obs_data_t *data = obs_data_create();

	/* Non-existent keys should return defaults */
	assert_string_equal(obs_data_get_string(data, "nonexistent"), "");
	assert_int_equal(obs_data_get_int(data, "nonexistent"), 0);
	assert_float_equal(obs_data_get_double(data, "nonexistent"), 0.0, 0.0001);
	assert_false(obs_data_get_bool(data, "nonexistent"));

	obs_data_release(data);
}

/* Test: obs_data_clear */
static void test_obs_data_clear(void **state)
{
	(void)state;

	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "key1", "value1");
	obs_data_set_string(data, "key2", "value2");

	assert_string_equal(obs_data_get_string(data, "key1"), "value1");

	obs_data_clear(data);

	assert_string_equal(obs_data_get_string(data, "key1"), "");
	assert_string_equal(obs_data_get_string(data, "key2"), "");

	obs_data_release(data);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_obs_data_create_destroy), cmocka_unit_test(test_obs_data_set_get_string),
		cmocka_unit_test(test_obs_data_set_get_int),    cmocka_unit_test(test_obs_data_set_get_double),
		cmocka_unit_test(test_obs_data_set_get_bool),   cmocka_unit_test(test_obs_data_nested),
		cmocka_unit_test(test_obs_data_array),          cmocka_unit_test(test_obs_data_json),
		cmocka_unit_test(test_obs_data_defaults),       cmocka_unit_test(test_obs_data_clear),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
