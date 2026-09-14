cmake_minimum_required(VERSION 3.25)
project(base64-benchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Release)
enable_testing()

# Get googletest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        b514bdc898e2951020cbdca1304b75f5950d1f59
  SYSTEM
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set_target_properties(gtest PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(gtest_main PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(gmock PROPERTIES CXX_CLANG_TIDY "")
set_target_properties(gmock_main PROPERTIES CXX_CLANG_TIDY "")

add_subdirectory(./libs/base64pp)
add_subdirectory(./libs/base64-aklomp)
add_subdirectory(./libs/etl)

include_directories(libs)

aux_source_directory(./src/ CTR_SOURCES)
aux_source_directory(./src/adp-gmbh.ch CTR_SOURCES)
aux_source_directory(./src/apache CTR_SOURCES)
aux_source_directory(./src/arduino CTR_SOURCES)
aux_source_directory(./src/base64pp CTR_SOURCES)
aux_source_directory(./src/base64-aklomp CTR_SOURCES)
aux_source_directory(./src/base64-tl CTR_SOURCES)
aux_source_directory(./src/DaedalusAlpha CTR_SOURCES)
aux_source_directory(./src/ElegantDice CTR_SOURCES)
aux_source_directory(./src/gnome CTR_SOURCES)
aux_source_directory(./src/InternetSoftwareConsortium CTR_SOURCES)
aux_source_directory(./src/JouniMalinen CTR_SOURCES)
aux_source_directory(./src/libb64 CTR_SOURCES)
aux_source_directory(./src/libcurl CTR_SOURCES)
aux_source_directory(./src/LihO CTR_SOURCES)
aux_source_directory(./src/ManuelMartinez CTR_SOURCES)
aux_source_directory(./src/NibbleAndAHalf CTR_SOURCES)
aux_source_directory(./src/omnifarious CTR_SOURCES)
aux_source_directory(./src/polfosol CTR_SOURCES)
aux_source_directory(./src/user152949 CTR_SOURCES)
aux_source_directory(./src/wikibooks.org CTR_SOURCES)
aux_source_directory(./src/adition CTR_SOURCES)
aux_source_directory(./src/A.Hristov CTR_SOURCES)
aux_source_directory(./src/tomykaria CTR_SOURCES)
aux_source_directory(./src/stov-180947 CTR_SOURCES)
aux_source_directory(./src/libb64/libb64/src C_SOURCES)
list(APPEND CTR_SOURCES ./src/GaspardPetit/test_gaspardpetit.cpp)
aux_source_directory(./src/picobase64 CTR_SOURCES)
aux_source_directory(./src/simdutf CTR_SOURCES)
aux_source_directory(./src/etlcpp CTR_SOURCES)

aux_source_directory(./libs/chromium/modp_b64 CTR_SOURCES)
aux_source_directory(./src/modp_b64_chromium CTR_SOURCES)

include_directories(libs/cppcodec)
aux_source_directory(./src/cppcodec CTR_SOURCES)

include_directories(libs/boost/utility/include)
include_directories(libs/boost/serialization/include)
include_directories(libs/boost/config/include)
include_directories(libs/boost/detail/include)
include_directories(libs/boost/preprocessor/include)
include_directories(libs/boost/core/include)
include_directories(libs/boost/type_traits/include)
include_directories(libs/boost/iterator/include)
include_directories(libs/boost/mpl/include)
include_directories(libs/boost/static_assert/include)
include_directories(libs/boost/assert/include)
include_directories(libs/boost/mp11/include)
aux_source_directory(./src/boost CTR_SOURCES)

set_source_files_properties( ${CTR_SOURCES} PROPERTIES LANGUAGE CXX )
set_source_files_properties( ${C_SOURCES} PROPERTIES LANGUAGE C )

# Compile only the AVX2 backend with AVX2 enabled; dispatch stays baseline x86-64.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND
   CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$" AND
   CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_library(gaspardpetit_base64 STATIC
    libs/gaspardpetit/base64/base64.c
    libs/gaspardpetit/base64/base64_avx2.c)
  set_source_files_properties(libs/gaspardpetit/base64/base64_avx2.c
    PROPERTIES COMPILE_OPTIONS "-mavx2")
  # Klomp exports the same C names; isolate this library within the survey.
  set(gaspardpetit_symbols
    base64_encode=gaspardpetit_encode
    base64_decode=gaspardpetit_decode)
  target_compile_definitions(gaspardpetit_base64 PRIVATE ${gaspardpetit_symbols})
  set_source_files_properties(src/GaspardPetit/test_gaspardpetit.cpp
    PROPERTIES COMPILE_DEFINITIONS "GASPARDPETIT_COMPILED;${gaspardpetit_symbols}")
endif()

add_executable(${PROJECT_NAME} ${CTR_SOURCES} ${C_SOURCES})
if(TARGET gaspardpetit_base64)
  target_link_libraries(${PROJECT_NAME} PRIVATE gaspardpetit_base64)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main base64pp base64 etl::etl)

add_test(NAME unit-tests COMMAND ${PROJECT_NAME} --tests-only)
set_tests_properties(unit-tests PROPERTIES LABELS unit)

add_test(NAME benchmarks COMMAND ${PROJECT_NAME})
set_tests_properties(benchmarks PROPERTIES LABELS benchmark)
