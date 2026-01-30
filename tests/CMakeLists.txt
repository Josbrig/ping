add_executable(statistics_aggregator_tests
    statistics_aggregator_tests.cpp
    ../src/statistics_aggregator.cpp
)

target_include_directories(statistics_aggregator_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_test(NAME statistics_aggregator_tests COMMAND statistics_aggregator_tests)
