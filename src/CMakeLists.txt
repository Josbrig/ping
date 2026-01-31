## Core source files for the pingstats executable
target_sources(pingstats PRIVATE
    main.cpp
    platform_ping_backend_factory.cpp
    ping_session.cpp
    statistics_aggregator.cpp
    console_view_impl.cpp
    csv_exporter.cpp
    json_exporter.cpp
)

## Platform-specific backend source selection
if(APPLE)
    target_sources(pingstats PRIVATE
        platform_ping_backend_macos.cpp
    )
elseif(UNIX)
    target_sources(pingstats PRIVATE
        platform_ping_backend_linux.cpp
    )
elseif(WIN32)
    target_sources(pingstats PRIVATE
        platform_ping_backend_windows.cpp
    )
    target_link_libraries(pingstats PRIVATE iphlpapi ws2_32)
endif()

## Public headers live under include/
target_include_directories(pingstats PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

## Enforce C++20 without compiler-specific extensions
set_target_properties(pingstats PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
