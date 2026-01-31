## Fetch Catch2 from internal mirror
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY http://raspirepo/mirrors/Catch2.git
    GIT_TAG v3.5.3
)
FetchContent_MakeAvailable(Catch2)

## Unit tests for statistics aggregation (reuses production source)
add_executable(statistics_aggregator_tests
    statistics_aggregator_tests.cpp
    ../src/statistics_aggregator.cpp
)

target_link_libraries(statistics_aggregator_tests PRIVATE
    Catch2::Catch2WithMain
)

target_include_directories(statistics_aggregator_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

include(CTest)
include(Catch)
catch_discover_tests(statistics_aggregator_tests)

## Workflow tests for ping session + aggregator
add_executable(ping_workflow_tests
    ping_workflow_tests.cpp
    ../src/ping_session.cpp
    ../src/statistics_aggregator.cpp
)

target_link_libraries(ping_workflow_tests PRIVATE
    Catch2::Catch2WithMain
)

target_include_directories(ping_workflow_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

catch_discover_tests(ping_workflow_tests)

## Integration tests for platform ping backend (Windows)
add_executable(integration_ping_tests
    integration_ping_tests.cpp
    ../src/platform_ping_backend_factory.cpp
    ../src/platform_ping_backend_windows.cpp
)

target_link_libraries(integration_ping_tests PRIVATE
    Catch2::Catch2WithMain
    iphlpapi
    ws2_32
)

target_include_directories(integration_ping_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_compile_definitions(integration_ping_tests PRIVATE PINGSTATS_INTEGRATION_HOST="simbrig.eu")

catch_discover_tests(integration_ping_tests)
