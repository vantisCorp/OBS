/******************************************************************************
    Copyright (C) 2026 vantisCorp

    Unit tests for source-cache.h
    
    Tests the source render cache helper functions used for
    optimizing hot paths by reducing property lookups.

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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "util/source-cache.h"

/* Mock obs_source structure for testing - matches internal structure */
struct obs_source {
    bool enabled;
    bool active;
    bool showing;
    bool removed;
    
    struct {
        enum obs_source_type type;
        uint32_t output_flags;
    } info;
    
    struct {
        size_t num;
    } filters;
    
    bool rendering_filter;
};

/* Test context */
struct test_ctx {
    obs_source source;
    source_cached_props_t cache;
};

/* Setup function */
static int setup(void **state) {
    struct test_ctx *ctx = malloc(sizeof(struct test_ctx));
    if (!ctx)
        return -1;
    
    memset(&ctx->source, 0, sizeof(ctx->source));
    memset(&ctx->cache, 0, sizeof(ctx->cache));
    *state = ctx;
    return 0;
}

/* Teardown function */
static int teardown(void **state) {
    free(*state);
    return 0;
}

/* Test: Basic caching of properties */
static void test_basic_property_caching(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->enabled = true;
    src->active = true;
    src->showing = true;
    src->removed = false;
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_AUDIO;
    
    source_cache_props(src, cache);
    
    assert_true(cache->enabled);
    assert_true(cache->active);
    assert_true(cache->showing);
    assert_false(cache->removed);
    assert_int_equal(cache->type, OBS_SOURCE_TYPE_INPUT);
    assert_int_equal(cache->output_flags, OBS_SOURCE_VIDEO | OBS_SOURCE_AUDIO);
}

/* Test: Null source handling */
static void test_null_source(void **state) {
    struct test_ctx *ctx = *state;
    source_cached_props_t *cache = &ctx->cache;
    
    source_cache_props(nullptr, cache);
    
    /* Cache should remain uninitialized */
    assert_false(cache->enabled);
    assert_false(cache->active);
}

/* Test: Null cache handling */
static void test_null_cache(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    
    src->enabled = true;
    src->active = true;
    
    /* Should not crash */
    source_cache_props(src, nullptr);
}

/* Test: Derived properties for input */
static void test_derived_input(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_ASYNC;
    
    source_cache_props(src, cache);
    
    assert_true(cache->is_input);
    assert_false(cache->is_filter);
    assert_false(cache->is_transition);
    assert_false(cache->is_scene);
    assert_true(cache->async_source);
    assert_true(cache->has_video);
}

/* Test: Derived properties for filter */
static void test_derived_filter(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_FILTER;
    src->info.output_flags = OBS_SOURCE_VIDEO;
    
    source_cache_props(src, cache);
    
    assert_true(cache->is_filter);
    assert_false(cache->is_input);
    assert_false(cache->is_transition);
    assert_false(cache->is_scene);
}

/* Test: Derived properties for transition */
static void test_derived_transition(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_TRANSITION;
    src->info.output_flags = OBS_SOURCE_VIDEO;
    
    source_cache_props(src, cache);
    
    assert_true(cache->is_transition);
    assert_false(cache->is_filter);
    assert_false(cache->is_input);
    assert_false(cache->is_scene);
}

/* Test: Derived properties for scene */
static void test_derived_scene(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_SCENE;
    src->info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW;
    
    source_cache_props(src, cache);
    
    assert_true(cache->is_scene);
    assert_false(cache->is_filter);
    assert_false(cache->is_input);
    assert_false(cache->is_transition);
}

/* Test: has_video flag */
static void test_has_video(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_VIDEO;
    
    source_cache_props(src, cache);
    assert_true(cache->has_video);
    
    src->info.output_flags = OBS_SOURCE_AUDIO;
    source_cache_props(src, cache);
    assert_false(cache->has_video);
}

/* Test: has_audio flag */
static void test_has_audio(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_AUDIO;
    
    source_cache_props(src, cache);
    assert_true(cache->has_audio);
    
    src->info.output_flags = OBS_SOURCE_VIDEO;
    source_cache_props(src, cache);
    assert_false(cache->has_audio);
}

/* Test: async_source requires INPUT type and ASYNC flag */
static void test_async_source(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    /* Input with ASYNC - should be async_source */
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_ASYNC;
    
    source_cache_props(src, cache);
    assert_true(cache->async_source);
    
    /* Filter with ASYNC - should NOT be async_source */
    src->info.type = OBS_SOURCE_TYPE_FILTER;
    source_cache_props(src, cache);
    assert_false(cache->async_source);
    
    /* Input without ASYNC - should NOT be async_source */
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_VIDEO;
    source_cache_props(src, cache);
    assert_false(cache->async_source);
}

/* Test: source_can_render_video - valid video source */
static void test_can_render_video_valid(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_VIDEO;
    
    source_cache_props(src, cache);
    assert_true(source_can_render_video(cache));
}

/* Test: source_can_render_video - filter cannot render */
static void test_can_render_video_filter(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_FILTER;
    src->info.output_flags = OBS_SOURCE_VIDEO;
    
    source_cache_props(src, cache);
    assert_false(source_can_render_video(cache));
}

/* Test: source_can_render_video - audio only */
static void test_can_render_video_audio_only(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_AUDIO;
    
    source_cache_props(src, cache);
    assert_false(source_can_render_video(cache));
}

/* Test: source_can_render_video - null cache */
static void test_can_render_video_null(void **state) {
    (void)state;
    assert_false(source_can_render_video(nullptr));
}

/* Test: source_should_skip_render - removed source */
static void test_skip_render_removed(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->removed = true;
    src->enabled = true;
    
    source_cache_props(src, cache);
    assert_true(source_should_skip_render(cache));
}

/* Test: source_should_skip_render - disabled source */
static void test_skip_render_disabled(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->removed = false;
    src->enabled = false;
    
    source_cache_props(src, cache);
    assert_true(source_should_skip_render(cache));
}

/* Test: source_should_skip_render - valid source */
static void test_skip_render_valid(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->removed = false;
    src->enabled = true;
    
    source_cache_props(src, cache);
    assert_false(source_should_skip_render(cache));
}

/* Test: source_should_skip_render - null cache */
static void test_skip_render_null(void **state) {
    (void)state;
    assert_true(source_should_skip_render(nullptr));
}

/* Test: source_needs_async_update - async input */
static void test_needs_async_valid(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_ASYNC;
    
    source_cache_props(src, cache);
    assert_true(source_needs_async_update(cache));
}

/* Test: source_needs_async_update - non-async */
static void test_needs_async_non_async(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_VIDEO;
    
    source_cache_props(src, cache);
    assert_false(source_needs_async_update(cache));
}

/* Test: source_has_filters - with filters */
static void test_has_filters_with(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->filters.num = 3;
    src->rendering_filter = false;
    
    assert_true(source_has_filters(src, cache));
}

/* Test: source_has_filters - no filters */
static void test_has_filters_without(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->filters.num = 0;
    src->rendering_filter = false;
    
    assert_false(source_has_filters(src, cache));
}

/* Test: source_has_filters - rendering filter */
static void test_has_filters_rendering(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    src->filters.num = 3;
    src->rendering_filter = true;
    
    assert_false(source_has_filters(src, cache));
}

/* Test: source_has_filters - null source */
static void test_has_filters_null(void **state) {
    struct test_ctx *ctx = *state;
    source_cached_props_t *cache = &ctx->cache;
    
    assert_false(source_has_filters(nullptr, cache));
}

/* Test: Complete workflow simulation */
static void test_workflow_simulation(void **state) {
    struct test_ctx *ctx = *state;
    obs_source *src = &ctx->source;
    source_cached_props_t *cache = &ctx->cache;
    
    /* Simulate a typical video input source */
    src->enabled = true;
    src->active = true;
    src->showing = true;
    src->removed = false;
    src->info.type = OBS_SOURCE_TYPE_INPUT;
    src->info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_AUDIO;
    src->filters.num = 2;
    src->rendering_filter = false;
    
    source_cache_props(src, cache);
    
    /* Check workflow decisions */
    assert_false(source_should_skip_render(cache));
    assert_true(source_can_render_video(cache));
    assert_false(source_needs_async_update(cache));
    assert_true(source_has_filters(src, cache));
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_basic_property_caching, setup, teardown),
        cmocka_unit_test_setup_teardown(test_null_source, setup, teardown),
        cmocka_unit_test_setup_teardown(test_null_cache, setup, teardown),
        cmocka_unit_test_setup_teardown(test_derived_input, setup, teardown),
        cmocka_unit_test_setup_teardown(test_derived_filter, setup, teardown),
        cmocka_unit_test_setup_teardown(test_derived_transition, setup, teardown),
        cmocka_unit_test_setup_teardown(test_derived_scene, setup, teardown),
        cmocka_unit_test_setup_teardown(test_has_video, setup, teardown),
        cmocka_unit_test_setup_teardown(test_has_audio, setup, teardown),
        cmocka_unit_test_setup_teardown(test_async_source, setup, teardown),
        cmocka_unit_test_setup_teardown(test_can_render_video_valid, setup, teardown),
        cmocka_unit_test_setup_teardown(test_can_render_video_filter, setup, teardown),
        cmocka_unit_test_setup_teardown(test_can_render_video_audio_only, setup, teardown),
        cmocka_unit_test_setup_teardown(test_can_render_video_null, setup, teardown),
        cmocka_unit_test_setup_teardown(test_skip_render_removed, setup, teardown),
        cmocka_unit_test_setup_teardown(test_skip_render_disabled, setup, teardown),
        cmocka_unit_test_setup_teardown(test_skip_render_valid, setup, teardown),
        cmocka_unit_test_setup_teardown(test_skip_render_null, setup, teardown),
        cmocka_unit_test_setup_teardown(test_needs_async_valid, setup, teardown),
        cmocka_unit_test_setup_teardown(test_needs_async_non_async, setup, teardown),
        cmocka_unit_test_setup_teardown(test_has_filters_with, setup, teardown),
        cmocka_unit_test_setup_teardown(test_has_filters_without, setup, teardown),
        cmocka_unit_test_setup_teardown(test_has_filters_rendering, setup, teardown),
        cmocka_unit_test_setup_teardown(test_has_filters_null, setup, teardown),
        cmocka_unit_test_setup_teardown(test_workflow_simulation, setup, teardown),
    };
    
    return cmocka_run_group_tests(tests, NULL, NULL);
}