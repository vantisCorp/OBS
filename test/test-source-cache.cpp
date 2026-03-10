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

#include <gtest/gtest.h>
#include <cstring>
#include "util/source-cache.h"

/* Mock obs_source structure for testing */
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

/* Test fixture for source cache tests */
class SourceCacheTest : public ::testing::Test {
protected:
	obs_source source;
	source_cached_props_t cache;

	void SetUp() override
	{
		memset(&source, 0, sizeof(source));
		memset(&cache, 0, sizeof(cache));
	}
};

/* Test: Basic caching of properties */
TEST_F(SourceCacheTest, BasicPropertyCaching)
{
	source.enabled = true;
	source.active = true;
	source.showing = true;
	source.removed = false;
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_AUDIO;

	source_cache_props(&source, &cache);

	EXPECT_TRUE(cache.enabled);
	EXPECT_TRUE(cache.active);
	EXPECT_TRUE(cache.showing);
	EXPECT_FALSE(cache.removed);
	EXPECT_EQ(cache.type, OBS_SOURCE_TYPE_INPUT);
	EXPECT_EQ(cache.output_flags, (uint32_t)(OBS_SOURCE_VIDEO | OBS_SOURCE_AUDIO));
}

/* Test: Null source handling */
TEST_F(SourceCacheTest, NullSourceHandling)
{
	source_cache_props(nullptr, &cache);

	/* Cache should remain uninitialized (all false/zero) */
	EXPECT_FALSE(cache.enabled);
	EXPECT_FALSE(cache.active);
}

/* Test: Null cache handling */
TEST_F(SourceCacheTest, NullCacheHandling)
{
	source.enabled = true;
	source.active = true;

	/* Should not crash */
	source_cache_props(&source, nullptr);
}

/* Test: Derived properties - input type */
TEST_F(SourceCacheTest, DerivedPropertiesInput)
{
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_ASYNC;

	source_cache_props(&source, &cache);

	EXPECT_TRUE(cache.is_input);
	EXPECT_FALSE(cache.is_filter);
	EXPECT_FALSE(cache.is_transition);
	EXPECT_FALSE(cache.is_scene);
	EXPECT_TRUE(cache.async_source);
	EXPECT_TRUE(cache.has_video);
}

/* Test: Derived properties - filter type */
TEST_F(SourceCacheTest, DerivedPropertiesFilter)
{
	source.info.type = OBS_SOURCE_TYPE_FILTER;
	source.info.output_flags = OBS_SOURCE_VIDEO;

	source_cache_props(&source, &cache);

	EXPECT_TRUE(cache.is_filter);
	EXPECT_FALSE(cache.is_input);
	EXPECT_FALSE(cache.is_transition);
	EXPECT_FALSE(cache.is_scene);
}

/* Test: Derived properties - transition type */
TEST_F(SourceCacheTest, DerivedPropertiesTransition)
{
	source.info.type = OBS_SOURCE_TYPE_TRANSITION;
	source.info.output_flags = OBS_SOURCE_VIDEO;

	source_cache_props(&source, &cache);

	EXPECT_TRUE(cache.is_transition);
	EXPECT_FALSE(cache.is_filter);
	EXPECT_FALSE(cache.is_input);
	EXPECT_FALSE(cache.is_scene);
}

/* Test: Derived properties - scene type */
TEST_F(SourceCacheTest, DerivedPropertiesScene)
{
	source.info.type = OBS_SOURCE_TYPE_SCENE;
	source.info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW;

	source_cache_props(&source, &cache);

	EXPECT_TRUE(cache.is_scene);
	EXPECT_FALSE(cache.is_filter);
	EXPECT_FALSE(cache.is_input);
	EXPECT_FALSE(cache.is_transition);
}

/* Test: has_video derived from output flags */
TEST_F(SourceCacheTest, HasVideoFlag)
{
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_VIDEO;

	source_cache_props(&source, &cache);
	EXPECT_TRUE(cache.has_video);

	source.info.output_flags = OBS_SOURCE_AUDIO;
	source_cache_props(&source, &cache);
	EXPECT_FALSE(cache.has_video);
}

/* Test: has_audio derived from output flags */
TEST_F(SourceCacheTest, HasAudioFlag)
{
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_AUDIO;

	source_cache_props(&source, &cache);
	EXPECT_TRUE(cache.has_audio);

	source.info.output_flags = OBS_SOURCE_VIDEO;
	source_cache_props(&source, &cache);
	EXPECT_FALSE(cache.has_audio);
}

/* Test: async_source requires both INPUT type and ASYNC flag */
TEST_F(SourceCacheTest, AsyncSourceRequiresInputAndAsync)
{
	/* Input with ASYNC - should be async_source */
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_ASYNC;

	source_cache_props(&source, &cache);
	EXPECT_TRUE(cache.async_source);

	/* Filter with ASYNC - should NOT be async_source */
	source.info.type = OBS_SOURCE_TYPE_FILTER;
	source_cache_props(&source, &cache);
	EXPECT_FALSE(cache.async_source);

	/* Input without ASYNC - should NOT be async_source */
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_VIDEO;
	source_cache_props(&source, &cache);
	EXPECT_FALSE(cache.async_source);
}

/* Test: source_can_render_video - valid video source */
TEST_F(SourceCacheTest, CanRenderVideoValid)
{
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_VIDEO;

	source_cache_props(&source, &cache);
	EXPECT_TRUE(source_can_render_video(&cache));
}

/* Test: source_can_render_video - filter cannot render */
TEST_F(SourceCacheTest, CanRenderVideoFilter)
{
	source.info.type = OBS_SOURCE_TYPE_FILTER;
	source.info.output_flags = OBS_SOURCE_VIDEO;

	source_cache_props(&source, &cache);
	EXPECT_FALSE(source_can_render_video(&cache));
}

/* Test: source_can_render_video - audio only source */
TEST_F(SourceCacheTest, CanRenderVideoAudioOnly)
{
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_AUDIO;

	source_cache_props(&source, &cache);
	EXPECT_FALSE(source_can_render_video(&cache));
}

/* Test: source_can_render_video - null cache */
TEST_F(SourceCacheTest, CanRenderVideoNullCache)
{
	EXPECT_FALSE(source_can_render_video(nullptr));
}

/* Test: source_should_skip_render - removed source */
TEST_F(SourceCacheTest, ShouldSkipRenderRemoved)
{
	source.removed = true;
	source.enabled = true;

	source_cache_props(&source, &cache);
	EXPECT_TRUE(source_should_skip_render(&cache));
}

/* Test: source_should_skip_render - disabled source */
TEST_F(SourceCacheTest, ShouldSkipRenderDisabled)
{
	source.removed = false;
	source.enabled = false;

	source_cache_props(&source, &cache);
	EXPECT_TRUE(source_should_skip_render(&cache));
}

/* Test: source_should_skip_render - valid source */
TEST_F(SourceCacheTest, ShouldSkipRenderValid)
{
	source.removed = false;
	source.enabled = true;

	source_cache_props(&source, &cache);
	EXPECT_FALSE(source_should_skip_render(&cache));
}

/* Test: source_should_skip_render - null cache */
TEST_F(SourceCacheTest, ShouldSkipRenderNullCache)
{
	EXPECT_TRUE(source_should_skip_render(nullptr));
}

/* Test: source_needs_async_update - async input */
TEST_F(SourceCacheTest, NeedsAsyncUpdateAsyncInput)
{
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_ASYNC;

	source_cache_props(&source, &cache);
	EXPECT_TRUE(source_needs_async_update(&cache));
}

/* Test: source_needs_async_update - non-async */
TEST_F(SourceCacheTest, NeedsAsyncUpdateNonAsync)
{
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_VIDEO;

	source_cache_props(&source, &cache);
	EXPECT_FALSE(source_needs_async_update(&cache));
}

/* Test: source_needs_async_update - filter with async */
TEST_F(SourceCacheTest, NeedsAsyncUpdateFilterAsync)
{
	source.info.type = OBS_SOURCE_TYPE_FILTER;
	source.info.output_flags = OBS_SOURCE_ASYNC;

	source_cache_props(&source, &cache);
	EXPECT_FALSE(source_needs_async_update(&cache));
}

/* Test: source_has_filters - with filters */
TEST_F(SourceCacheTest, HasFiltersWithFilters)
{
	source.filters.num = 3;
	source.rendering_filter = false;

	EXPECT_TRUE(source_has_filters(&source, &cache));
}

/* Test: source_has_filters - no filters */
TEST_F(SourceCacheTest, HasFiltersNoFilters)
{
	source.filters.num = 0;
	source.rendering_filter = false;

	EXPECT_FALSE(source_has_filters(&source, &cache));
}

/* Test: source_has_filters - rendering filter */
TEST_F(SourceCacheTest, HasFiltersRenderingFilter)
{
	source.filters.num = 3;
	source.rendering_filter = true;

	EXPECT_FALSE(source_has_filters(&source, &cache));
}

/* Test: source_has_filters - null source */
TEST_F(SourceCacheTest, HasFiltersNullSource)
{
	EXPECT_FALSE(source_has_filters(nullptr, &cache));
}

/* Test: Complete workflow simulation */
TEST_F(SourceCacheTest, WorkflowSimulation)
{
	/* Simulate a typical video input source */
	source.enabled = true;
	source.active = true;
	source.showing = true;
	source.removed = false;
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_AUDIO;
	source.filters.num = 2;
	source.rendering_filter = false;

	source_cache_props(&source, &cache);

	/* Check workflow decisions */
	EXPECT_FALSE(source_should_skip_render(&cache));
	EXPECT_TRUE(source_can_render_video(&cache));
	EXPECT_FALSE(source_needs_async_update(&cache));
	EXPECT_TRUE(source_has_filters(&source, &cache));
}

/* Test: Complete workflow - async source */
TEST_F(SourceCacheTest, WorkflowAsyncSource)
{
	source.enabled = true;
	source.active = true;
	source.showing = true;
	source.removed = false;
	source.info.type = OBS_SOURCE_TYPE_INPUT;
	source.info.output_flags = OBS_SOURCE_ASYNC | OBS_SOURCE_VIDEO;
	source.filters.num = 0;

	source_cache_props(&source, &cache);

	EXPECT_FALSE(source_should_skip_render(&cache));
	EXPECT_TRUE(source_can_render_video(&cache));
	EXPECT_TRUE(source_needs_async_update(&cache));
	EXPECT_FALSE(source_has_filters(&source, &cache));
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
