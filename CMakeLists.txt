cmake_minimum_required(VERSION 3.21 FATAL_ERROR) # ROCM support
project(mitransform CXX)

if ("x" STREQUAL "x${CMAKE_BUILD_TYPE}")
set(CMAKE_BUILD_TYPE Release)
endif()

if (UNIX)
# Use $ORIGIN or @loader_path, which allow to pick up
# dependencies relative to the executable folder
if (APPLE)
set(CMAKE_INSTALL_RPATH "@loader_path")
else()
set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()
endif()

if(NOT DEFINED ROCM_PATH)
        if(NOT DEFINED ENV{ROCM_PATH})
                set(ROCM_PATH "/opt/rocm" CACHE PATH "Path to which ROCM has been installed")
        else()
                set(ROCM_PATH $ENV{ROCM_PATH} CACHE PATH "Path to which ROCM has been installed")
        endif()
endif()
set(CMAKE_PREFIX_PATH "${ROCM_PATH}" ${CMAKE_PREFIX_PATH})

# Must be here to workaround error "clangrt builtins lib not found"
# However, we don't use HIP compilers, the targets below are using
# system-provided toolchain (e.g. GCC).
enable_language(HIP)

find_package(ROCBLAS REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(amd_comgr REQUIRED)

add_subdirectory(ThirdParty/mitransform_kernels)
add_subdirectory(ThirdParty/gputt)

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} STATIC ${SRCS})
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/json/include)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_kernels roc::rocblas Boost::filesystem amd_comgr gputt)

add_executable(${PROJECT_NAME}_test ${CMAKE_CURRENT_SOURCE_DIR}/src/test/mitransform_test.cpp)
target_compile_features(${PROJECT_NAME}_test PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/test")
target_link_libraries(${PROJECT_NAME}_test PUBLIC ${PROJECT_NAME})

