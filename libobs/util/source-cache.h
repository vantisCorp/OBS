/******************************************************************************
    Copyright (C) 2026 vantisCorp

    Source render cache helpers for OBS Studio.
    
    Provides inline functions to cache frequently accessed source properties
    in local variables, reducing memory access in hot paths.
    
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

#include "obs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Cached source properties structure.
 * 
 * Used to cache frequently accessed properties in local variables
 * to reduce pointer chasing in hot paths.
 */
struct source_cached_props {
    bool enabled;
    bool active;
    bool showing;
    bool removed;
    bool has_video;
    bool has_audio;
    bool is_filter;
    bool is_input;
    bool is_transition;
    bool is_scene;
    bool async_source;
    uint32_t output_flags;
    enum obs_source_type type;
};

typedef struct source_cached_props source_cached_props_t;

/**
 * Cache source properties into a local structure.
 * 
 * This function should be called once at the beginning of a hot path
 * to cache all frequently accessed properties. Subsequent checks
 * should use the cached values.
 * 
 * @param source Source to cache properties from
 * @param cache Structure to store cached properties
 */
static inline void source_cache_props(obs_source_t *source, source_cached_props_t *cache)
{
    if (!source || !cache)
        return;
    
    /* Cache basic state flags */
    cache->enabled = source->enabled;
    cache->active = source->active;
    cache->showing = source->showing;
    cache->removed = source->removed;
    
    /* Cache type info */
    cache->type = source->info.type;
    cache->output_flags = source->info.output_flags;
    
    /* Derived properties - computed once */
    cache->is_filter = (cache->type == OBS_SOURCE_TYPE_FILTER);
    cache->is_input = (cache->type == OBS_SOURCE_TYPE_INPUT);
    cache->is_transition = (cache->type == OBS_SOURCE_TYPE_TRANSITION);
    cache->is_scene = (cache->type == OBS_SOURCE_TYPE_SCENE);
    
    cache->has_video = (cache->output_flags & OBS_SOURCE_VIDEO) != 0;
    cache->has_audio = (cache->output_flags & OBS_SOURCE_AUDIO) != 0;
    cache->async_source = cache->is_input && (cache->output_flags & OBS_SOURCE_ASYNC) != 0;
}

/**
 * Check if source can render video.
 * 
 * @param cache Cached properties
 * @return true if source can render video
 */
static inline bool source_can_render_video(const source_cached_props_t *cache)
{
    if (!cache)
        return false;
    
    return !cache->is_filter && cache->has_video;
}

/**
 * Check if source should skip rendering.
 * 
 * @param cache Cached properties
 * @return true if source should skip rendering
 */
static inline bool source_should_skip_render(const source_cached_props_t *cache)
{
    if (!cache)
        return true;
    
    return cache->removed || !cache->enabled;
}

/**
 * Check if async video update is needed.
 * 
 * @param cache Cached properties
 * @return true if async update is needed
 */
static inline bool source_needs_async_update(const source_cached_props_t *cache)
{
    if (!cache)
        return false;
    
    return cache->async_source && !cache->is_filter;
}

/**
 * Check if source has filters to render.
 * 
 * @param source Source to check
 * @param cache Cached properties
 * @return true if filters should be rendered
 */
static inline bool source_has_filters(obs_source_t *source, const source_cached_props_t *cache)
{
    if (!source || !cache)
        return false;
    
    return source->filters.num > 0 && !source->rendering_filter;
}

#ifdef __cplusplus
}
#endif