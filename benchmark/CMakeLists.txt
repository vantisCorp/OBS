# =============================================================================
# OBS Studio Benchmark Suite
# vantisCorp fork - Phase 3 Performance Improvements
#
# Uses Google Benchmark for performance testing of critical paths
# =============================================================================

option(ENABLE_BENCHMARKS "Build performance benchmarks" OFF)

if(ENABLE_BENCHMARKS)
    find_package(benchmark CONFIG REQUIRED)

    # Benchmark for obs_data operations
    add_executable(bench_obs_data
        bench_obs_data.cpp
    )
    target_link_libraries(bench_obs_data
        PRIVATE
            OBS::libobs
            benchmark::benchmark
    )

    # Benchmark for source rendering
    add_executable(bench_obs_source
        bench_obs_source.cpp
    )
    target_link_libraries(bench_obs_source
        PRIVATE
            OBS::libobs
            benchmark::benchmark
    )

    # Benchmark for audio processing
    add_executable(bench_obs_audio
        bench_obs_audio.cpp
    )
    target_link_libraries(bench_obs_audio
        PRIVATE
            OBS::libobs
            benchmark::benchmark
    )

    # Benchmark for video frame processing
    add_executable(bench_obs_video
        bench_obs_video.cpp
    )
    target_link_libraries(bench_obs_video
        PRIVATE
            OBS::libobs
            benchmark::benchmark
    )

    # Add custom target to run all benchmarks
    add_custom_target(run_benchmarks
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS bench_obs_data bench_obs_source bench_obs_audio bench_obs_video
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    message(STATUS "Benchmarks enabled")
endif()