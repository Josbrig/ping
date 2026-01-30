include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY http://raspirepo/mirrors/Catch2.git
    GIT_TAG v3.5.3
)
FetchContent_MakeAvailable(Catch2)

add_executable(statistics_aggregator_tests
    statistics_aggregator_tests.cpp
)

target_link_libraries(statistics_aggregator_tests PRIVATE
    Catch2::Catch2WithMain
    pingstats
)

target_include_directories(statistics_aggregator_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

include(CTest)
include(Catch)
catch_discover_tests(statistics_aggregator_tests)

add_executable(ping_workflow_tests
    ping_workflow_tests.cpp
)

target_link_libraries(ping_workflow_tests PRIVATE
    Catch2::Catch2WithMain
    pingstats
)

target_include_directories(ping_workflow_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

catch_discover_tests(ping_workflow_tests)
