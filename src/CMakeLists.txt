target_sources(pingstats PRIVATE
    main.cpp
    platform_ping_backend_factory.cpp
    ping_session.cpp
    statistics_aggregator.cpp
    console_view_impl.cpp
    csv_exporter.cpp
)

if(APPLE)
    target_sources(pingstats PRIVATE
        platform_ping_backend_macos.cpp
    )
elseif(UNIX)
    target_sources(pingstats PRIVATE
        platform_ping_backend_linux.cpp
    )
endif()

target_include_directories(pingstats PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set_target_properties(pingstats PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
