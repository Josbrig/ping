## Fetch Catch2 from internal mirror
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
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

## Integration tests for platform ping backend (Windows, Linux, macOS)
set(INTEGRATION_BACKEND_SRC "")
set(INTEGRATION_BACKEND_LIBS "")

if(WIN32)
    set(INTEGRATION_BACKEND_SRC ../src/platform_ping_backend_windows.cpp)
    set(INTEGRATION_BACKEND_LIBS iphlpapi ws2_32)
elseif(APPLE)
    set(INTEGRATION_BACKEND_SRC ../src/platform_ping_backend_macos.cpp)
elseif(UNIX)
    set(INTEGRATION_BACKEND_SRC ../src/platform_ping_backend_linux.cpp)
endif()

if(INTEGRATION_BACKEND_SRC)
    add_executable(integration_ping_tests
        integration_ping_tests.cpp
        ../src/platform_ping_backend_factory.cpp
        ${INTEGRATION_BACKEND_SRC}
    )

    target_link_libraries(integration_ping_tests PRIVATE
        Catch2::Catch2WithMain
        ${INTEGRATION_BACKEND_LIBS}
    )

    target_include_directories(integration_ping_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

    target_compile_definitions(integration_ping_tests PRIVATE PINGSTATS_INTEGRATION_HOST="simbrig.eu")

    catch_discover_tests(integration_ping_tests
        PROPERTIES LABELS "network"
    )
endif()

## Aggregated build target for all tests (exposed to parent scope)
set(PINGSTATS_TEST_TARGETS
    statistics_aggregator_tests
    ping_workflow_tests
)
if(INTEGRATION_BACKEND_SRC)
    list(APPEND PINGSTATS_TEST_TARGETS integration_ping_tests)
endif()

add_custom_target(pingstats_tests ALL
    DEPENDS ${PINGSTATS_TEST_TARGETS}
    COMMENT "Build all pingstats test executables"
)

# Export the list to parent scope so top-level can depend if desired
set(PINGSTATS_TEST_TARGETS "${PINGSTATS_TEST_TARGETS}" PARENT_SCOPE)
