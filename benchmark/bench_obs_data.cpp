/*
 * Performance Benchmarks for obs_data
 * vantisCorp fork - Phase 3 Performance Improvements
 *
 * Measures performance of critical data operations
 */

#include <benchmark/benchmark.h>

#include <obs-module.h>
#include <obs-data.h>

// =============================================================================
// obs_data creation/destruction benchmarks
// =============================================================================

static void BM_obs_data_create_destroy(benchmark::State &state)
{
	for (auto _ : state) {
		obs_data_t *data = obs_data_create();
		benchmark::DoNotOptimize(data);
		obs_data_release(data);
	}
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_data_create_destroy);

// =============================================================================
// String operations benchmarks
// =============================================================================

static void BM_obs_data_set_string(benchmark::State &state)
{
	obs_data_t *data = obs_data_create();
	const char *key = "test_key";
	const char *value = "test_value_with_some_length";

	for (auto _ : state) {
		obs_data_set_string(data, key, value);
		benchmark::ClobberMemory();
	}

	obs_data_release(data);
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_data_set_string);

static void BM_obs_data_get_string(benchmark::State &state)
{
	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "test_key", "test_value_with_some_length");

	for (auto _ : state) {
		const char *value = obs_data_get_string(data, "test_key");
		benchmark::DoNotOptimize(value);
	}

	obs_data_release(data);
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_data_get_string);

// =============================================================================
// Numeric operations benchmarks
// =============================================================================

static void BM_obs_data_set_int(benchmark::State &state)
{
	obs_data_t *data = obs_data_create();

	for (auto _ : state) {
		obs_data_set_int(data, "int_key", state.iterations());
		benchmark::ClobberMemory();
	}

	obs_data_release(data);
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_data_set_int);

static void BM_obs_data_get_int(benchmark::State &state)
{
	obs_data_t *data = obs_data_create();
	obs_data_set_int(data, "int_key", 12345);

	for (auto _ : state) {
		long long value = obs_data_get_int(data, "int_key");
		benchmark::DoNotOptimize(value);
	}

	obs_data_release(data);
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_data_get_int);

// =============================================================================
// JSON serialization benchmarks
// =============================================================================

static void BM_obs_data_to_json(benchmark::State &state)
{
	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "name", "test_object");
	obs_data_set_int(data, "value", 42);
	obs_data_set_bool(data, "enabled", true);
	obs_data_set_double(data, "ratio", 3.14159);

	for (auto _ : state) {
		const char *json = obs_data_get_json(data);
		benchmark::DoNotOptimize(json);
	}

	obs_data_release(data);
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_data_to_json);

static void BM_obs_data_from_json(benchmark::State &state)
{
	const char *json = R"({"name":"test","value":42,"enabled":true,"ratio":3.14159})";

	for (auto _ : state) {
		obs_data_t *data = obs_data_create_from_json(json);
		benchmark::DoNotOptimize(data);
		obs_data_release(data);
	}

	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_data_from_json);

// =============================================================================
// Nested objects benchmarks
// =============================================================================

static void BM_obs_data_nested_obj(benchmark::State &state)
{
	obs_data_t *parent = obs_data_create();

	for (auto _ : state) {
		obs_data_t *child = obs_data_create();
		obs_data_set_string(child, "child_key", "child_value");
		obs_data_set_obj(parent, "nested", child);
		obs_data_release(child);
		benchmark::ClobberMemory();
	}

	obs_data_release(parent);
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_data_nested_obj);

// =============================================================================
// Array operations benchmarks
// =============================================================================

static void BM_obs_data_array_push(benchmark::State &state)
{
	obs_data_array_t *array = obs_data_array_create();

	int i = 0;
	for (auto _ : state) {
		obs_data_t *item = obs_data_create();
		obs_data_set_int(item, "index", i++);
		obs_data_array_push_back(array, item);
		obs_data_release(item);
	}

	obs_data_array_release(array);
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_data_array_push);

static void BM_obs_data_array_iterate(benchmark::State &state)
{
	obs_data_array_t *array = obs_data_array_create();

	// Pre-populate array
	for (int i = 0; i < 100; i++) {
		obs_data_t *item = obs_data_create();
		obs_data_set_int(item, "index", i);
		obs_data_array_push_back(array, item);
		obs_data_release(item);
	}

	for (auto _ : state) {
		for (size_t i = 0; i < obs_data_array_count(array); i++) {
			obs_data_t *item = obs_data_array_item(array, i);
			benchmark::DoNotOptimize(item);
			obs_data_release(item);
		}
	}

	obs_data_array_release(array);
	state.SetItemsProcessed(state.iterations() * 100);
}
BENCHMARK(BM_obs_data_array_iterate);

// =============================================================================
// Large data benchmarks
// =============================================================================

static void BM_obs_data_large_json(benchmark::State &state)
{
	obs_data_t *data = obs_data_create();

	// Create a large object
	for (int i = 0; i < 100; i++) {
		char key[32];
		snprintf(key, sizeof(key), "key_%d", i);
		obs_data_set_int(data, key, i);
	}

	for (auto _ : state) {
		const char *json = obs_data_get_json(data);
		benchmark::DoNotOptimize(json);
	}

	obs_data_release(data);
	state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_obs_data_large_json);

// Main benchmark runner
BENCHMARK_MAIN();
