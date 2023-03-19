cmake_minimum_required(VERSION 3.14)

project("file-handle-closer")

if (NOT WIN32)
    message(FATAL_ERROR "This project only supports windows")
endif ()

# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

add_executable(${PROJECT_NAME}
        "src/main.cpp"
        "src/search_strategies/regex_search.cpp"
        "src/search_strategies/pattern_search.cpp"
        "src/file_handles.cpp"
        "src/utils/debug_privileges.cpp"
        "src/nt/nt_functions.cpp"
        )
target_link_libraries(${PROJECT_NAME} argparse)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX)

target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX UNICODE _UNICODE)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

