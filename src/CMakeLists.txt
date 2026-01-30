target_sources(pingstats PRIVATE
    main.cpp
    platform_ping_backend_linux.cpp
)

target_include_directories(pingstats PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set_target_properties(pingstats PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
