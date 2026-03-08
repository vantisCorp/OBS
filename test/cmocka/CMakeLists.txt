project(obs-cmocka)

find_package(CMocka CONFIG REQUIRED)

# =============================================================================
# Original Tests
# =============================================================================

# Serializer test
add_executable(test_serializer test_serializer.c)
target_include_directories(test_serializer PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_serializer PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})
add_test(test_serializer ${CMAKE_CURRENT_BINARY_DIR}/test_serializer)

# darray test
add_executable(test_darray test_darray.c)
target_include_directories(test_darray PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_darray PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})

add_test(test_darray ${CMAKE_CURRENT_BINARY_DIR}/test_darray)

# bitstream test
add_executable(test_bitstream test_bitstream.c)
target_include_directories(test_bitstream PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_bitstream PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})

add_test(test_bitstream ${CMAKE_CURRENT_BINARY_DIR}/test_bitstream)

# OS path test
add_executable(test_os_path test_os_path.c)
target_include_directories(test_os_path PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_os_path PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})

add_test(test_os_path ${CMAKE_CURRENT_BINARY_DIR}/test_os_path)

# =============================================================================
# New Tests - vantisCorp Phase 2
# =============================================================================

# obs-data test - data serialization
add_executable(test_obs_data test_obs_data.c)
target_include_directories(test_obs_data PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_obs_data PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})
add_test(test_obs_data ${CMAKE_CURRENT_BINARY_DIR}/test_obs_data)

# obs-source test - video/audio sources
add_executable(test_obs_source test_obs_source.c)
target_include_directories(test_obs_source PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_obs_source PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})
add_test(test_obs_source ${CMAKE_CURRENT_BINARY_DIR}/test_obs_source)

# obs-output test - recording/streaming outputs
add_executable(test_obs_output test_obs_output.c)
target_include_directories(test_obs_output PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_obs_output PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})
add_test(test_obs_output ${CMAKE_CURRENT_BINARY_DIR}/test_obs_output)

# =============================================================================
# New Tests - vantisCorp Phase 4
# =============================================================================

# source-cache test - cached source properties for hot path optimization
add_executable(test_source_cache test_source_cache.c)
target_include_directories(test_source_cache PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(test_source_cache PRIVATE OBS::libobs ${CMOCKA_LIBRARIES})
add_test(test_source_cache ${CMAKE_CURRENT_BINARY_DIR}/test_source_cache)

# Set test properties
set_tests_properties(test_obs_data test_obs_source test_obs_output test_source_cache
        PROPERTIES
        TIMEOUT 60
        LABELS "unit"
)
