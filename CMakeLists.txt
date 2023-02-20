cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
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

find_package(ROCBLAS REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(BZip2 REQUIRED)
find_package(amd_comgr REQUIRED)

add_subdirectory(ThirdParty/mitransform_kernels)

file(GLOB ${PROJECT_NAME}_SOURCE_FILES "src/*.cu" "src/*.cpp")
file(GLOB ${PROJECT_NAME}_test_SOURCE_FILES "src/test/*.cpp")

include(CheckLanguage)
check_language(HIP)
check_language(CUDA)

if(CMAKE_HIP_COMPILER)

# enable HIP
enable_language(HIP)

if(NOT DEFINED HIP_PATH)
    if(NOT DEFINED ENV{HIP_PATH})
        set(HIP_PATH "/opt/rocm/hip" CACHE PATH "Path to which HIP has been installed")
    else()
        set(HIP_PATH $ENV{HIP_PATH} CACHE PATH "Path to which HIP has been installed")
    endif()
endif()
set(CMAKE_MODULE_PATH "${HIP_PATH}/cmake" ${CMAKE_MODULE_PATH})
find_package(hip REQUIRED)

set(CMAKE_HIP_FLAGS_DEBUG "-ggdb -fstandalone-debug -Xclang -O0 -Xclang -gcodeview" ${CMAKE_HIP_FLAGS})

set_source_files_properties(${${PROJECT_NAME}_SOURCE_FILES} PROPERTIES LANGUAGE HIP)
set_source_files_properties(${${PROJECT_NAME}_test_SOURCE_FILES} PROPERTIES LANGUAGE HIP)

elseif(CMAKE_CUDA_COMPILER)

set(CMAKE_CUDA_FLAGS_DEBUG "-g -G -O0" ${CMAKE_CUDA_FLAGS})

else()

message(FATAL_ERROR "CUDA or HIP backend must be available, aborting")

endif()

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCE_FILES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_ARCHITECTURES native)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/json/include)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_kernels roc::rocblas Boost::filesystem SQLite::SQLite3 BZip2::BZip2 amd_comgr ${CMAKE_DL_LIBS})

add_executable(${PROJECT_NAME}_test ${${PROJECT_NAME}_test_SOURCE_FILES})
target_compile_features(${PROJECT_NAME}_test PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/test")
target_link_libraries(${PROJECT_NAME}_test PUBLIC ${PROJECT_NAME})

