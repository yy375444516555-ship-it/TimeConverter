file(GLOB SOFA_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_library(sofa STATIC ${SOFA_SOURCES})

target_include_directories(sofa PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)