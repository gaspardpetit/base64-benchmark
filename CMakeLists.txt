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
  GIT_TAG        063de7e9578f82b369302001269680b4b1553359
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

# TurboBase64 defaults to checking only a subset of SIMD blocks. Enable its
# full validation path so decoder benchmarks compare equivalent checked APIs.
foreach(turbo_object _b64_scalar _b64_v128 _b64_avx _b64_v256 _b64_v512)
  if(TARGET ${turbo_object})
    target_compile_definitions(${turbo_object} PRIVATE B64CHECK)
  endif()
endforeach()

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
aux_source_directory(./src/IMUtility CTR_SOURCES)
set_property(SOURCE ./src/IMUtility/test_polfosol_imutility.cpp
  APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/src/IMUtility")
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
include_directories(libs/libb64/include)
aux_source_directory(./libs/libb64/src C_SOURCES)
list(APPEND CTR_SOURCES ./src/GaspardPetit/test_gaspardpetit.cpp)
if((MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8) OR
   (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND
    CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$" AND
    CMAKE_C_COMPILER_ID MATCHES "GNU|Clang"))
  add_library(gaspardpetit_simd STATIC
    ./libs/gaspardpetit/base64/base64.c
    ./libs/gaspardpetit/base64/base64_avx2.c
  )
  if(MSVC)
    set_source_files_properties(./libs/gaspardpetit/base64/base64_avx2.c
      PROPERTIES COMPILE_OPTIONS "/arch:AVX2")
  else()
    set_source_files_properties(./libs/gaspardpetit/base64/base64_avx2.c
      PROPERTIES COMPILE_OPTIONS "-mavx2")
  endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND
       CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64)$")
  add_library(gaspardpetit_simd STATIC
    ./libs/gaspardpetit/base64/base64.c
    ./libs/gaspardpetit/base64/base64_neon.c
  )
endif()
if(TARGET gaspardpetit_simd)
  target_compile_definitions(gaspardpetit_simd PRIVATE
    base64_encode=gaspardpetit_simd_encode
    base64_decode=gaspardpetit_simd_decode
    base64_decode_unchecked=gaspardpetit_simd_decode_unchecked
  )
  list(APPEND CTR_SOURCES
    ./src/GaspardPetit/test_gaspardpetit_simd.cpp
  )
endif()
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

add_executable(${PROJECT_NAME} ${CTR_SOURCES} ${C_SOURCES})
if(TARGET gaspardpetit_simd)
  target_link_libraries(${PROJECT_NAME} PRIVATE gaspardpetit_simd)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main base64pp base64 etl::etl)

add_test(NAME unit-tests COMMAND ${PROJECT_NAME} --tests-only)
set_tests_properties(unit-tests PROPERTIES LABELS unit)

add_test(NAME benchmarks COMMAND ${PROJECT_NAME})
set_tests_properties(benchmarks PROPERTIES LABELS benchmark)
