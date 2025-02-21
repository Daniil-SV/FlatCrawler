include(FetchContent)

FetchContent_Declare(
    WorkshopCore
    GIT_REPOSITORY https://github.com/sc-workshop/Workshop-Core.git
    GIT_TAG main
)
FetchContent_MakeAvailable(WorkshopCore)

FetchContent_Declare(
    is_utf8
    GIT_REPOSITORY https://github.com/simdutf/is_utf8.git
    GIT_TAG v1.4.1
)
FetchContent_MakeAvailable(is_utf8)

include(WkArgsparse)
include(WkFlatbuffers)

# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h)
file(GLOB_RECURSE SOURCES source/*.cpp source/*.hpp)

set(TARGET FlatCrawler)

add_executable(${TARGET} ${HEADERS} ${SOURCES})

set_target_properties(${TARGET} PROPERTIES
    FOLDER SupercellSDK/tools
)

target_include_directories(${TARGET} PUBLIC source)

target_link_libraries(${TARGET} PRIVATE wk::core is_utf8)

wk_project_setup(${TARGET})
wk_include_argparse()
wk_include_flatbuffers()