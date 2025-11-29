cmake_minimum_required(VERSION 3.17)
project(OfflinePhotoEnhancer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Dependencies
# OpenCV
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/third_party/opencv/build")
find_package(OpenCV QUIET)
if(NOT OpenCV_FOUND)
    message(STATUS "OpenCV not found in third_party, trying system...")
    find_package(OpenCV REQUIRED)
endif()
message(STATUS "OpenCV found: ${OpenCV_LIBS}")

# ONNX Runtime
set(ONNXRUNTIME_ROOT "${CMAKE_SOURCE_DIR}/third_party/onnxruntime")
find_path(ONNXRUNTIME_INCLUDE_DIR onnxruntime_cxx_api.h
    HINTS "${ONNXRUNTIME_ROOT}/include"
)
find_library(ONNXRUNTIME_LIB onnxruntime
    HINTS "${ONNXRUNTIME_ROOT}/lib"
)
if(NOT ONNXRUNTIME_INCLUDE_DIR OR NOT ONNXRUNTIME_LIB)
    message(WARNING "ONNX Runtime not found in third_party. Please run setup_dependencies.bat or configure paths manually.")
endif()

include_directories(${OpenCV_INCLUDE_DIRS} ${ONNXRUNTIME_INCLUDE_DIR})
include_directories(src)

# Sources
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp" "src/core/*.hpp")
file(GLOB_RECURSE GUI_SOURCES "src/gui/*.cpp" "src/gui/*.hpp")

# CLI Executable
add_executable(enhancer-cli src/main_cli.cpp ${CORE_SOURCES})
target_link_libraries(enhancer-cli PRIVATE ${OpenCV_LIBS} ${ONNXRUNTIME_LIB})

# GUI Executable (Win32)
add_executable(enhancer-gui WIN32 src/main_gui.cpp ${CORE_SOURCES} ${GUI_SOURCES})
target_link_libraries(enhancer-gui PRIVATE ${OpenCV_LIBS} ${ONNXRUNTIME_LIB} comctl32 shlwapi)

# Tests
add_executable(run_tests tests/test_core.cpp ${CORE_SOURCES})
target_link_libraries(run_tests PRIVATE ${OpenCV_LIBS} ${ONNXRUNTIME_LIB})

# Copy DLLs to bin (Windows)
if(WIN32)
    add_custom_command(TARGET enhancer-cli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ONNXRUNTIME_ROOT}/lib/onnxruntime.dll"
        $<TARGET_FILE_DIR:enhancer-cli>
    )
    add_custom_command(TARGET enhancer-gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ONNXRUNTIME_ROOT}/lib/onnxruntime.dll"
        $<TARGET_FILE_DIR:enhancer-gui>
    )
endif()
