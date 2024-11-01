# 指定 cmake 最小版本
cmake_minimum_required(VERSION 3.5)

# 指定项目名称
project(cpputils
  VERSION 0.0.1
  LANGUAGES CXX C)

# 指定适用的操作系统
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "This project requires Linux operating system.")
endif()

# 设置编译提醒
if(NOT CMAKE_BUILD_TYPE AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Default BUILD_TYPE is Debug, other options are: RelWithDebInfo, Release, and MinSizeRel." FORCE)
endif()

# 设置 C++ 标准为 C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCXXCompilerFlag)
include(CheckSymbolExists)

check_symbol_exists(__func__ "" HAVE_FUNC)
if(HAVE_FUNC)
    add_definitions(-DHAVE_FUNC)
endif(HAVE_FUNC)
check_symbol_exists(__PRETTY_FUNCTION__ "" HAVE_PRETTY_FUNC)
if(HAVE_PRETTY_FUNC)
    add_definitions(-DHAVE_PRETTY_FUNC)
endif(HAVE_PRETTY_FUNC)

# 指定成果物路径
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# 指定文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/include)

# 设置库的源文件
set(SOURCES
  src/version.cc
  src/backtrace.cc
  src/dout.cc
  src/utime.cc
  src/clock.cc
  src/assert.cc
)

# 创建动态库
add_library(cpputils SHARED ${SOURCES})

# 创建静态库
add_library(cpputils-static STATIC ${SOURCES})

# 安装头文件
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cpputils"
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

# 安装库文件
install(TARGETS cpputils cpputils-static
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

# Used by googletest.
check_cxx_compiler_flag(-Wno-missing-field-initializers
                        CPPUTILS_HAVE_NO_MISSING_FIELD_INITIALIZERS)
check_cxx_compiler_flag(-Wno-implicit-int-float-conversion
                        CPPUTILS_HAVE_NO_IMPLICIT_INT_FLOAT_CONVERSION)

option(CPPUTILS_BUILD_TESTS "Build cpputils's own tests." ON)
if(CPPUTILS_BUILD_TESTS)

  enable_testing()

  # This project is tested using GoogleTest.
  add_subdirectory("third_party/googletest")

  # GoogleTest triggers a missing field initializers warning.
  if(CPPUTILS_HAVE_NO_MISSING_FIELD_INITIALIZERS)
    set_property(TARGET gtest
        APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
    set_property(TARGET gmock
        APPEND PROPERTY COMPILE_OPTIONS -Wno-missing-field-initializers)
  endif(CPPUTILS_HAVE_NO_MISSING_FIELD_INITIALIZERS)

  if(CPPUTILS_HAVE_NO_IMPLICIT_INT_FLOAT_CONVERSION)
    set_property(TARGET gtest
        APPEND PROPERTY COMPILE_OPTIONS -Wno-implicit-int-float-conversion)
  endif(CPPUTILS_HAVE_NO_IMPLICIT_INT_FLOAT_CONVERSION)

  add_executable(cpputils_unittest_miscs "")
  target_sources(cpputils_unittest_miscs
    PRIVATE
      "test/test_miscs.cc"
  )
  target_link_libraries(cpputils_unittest_miscs cpputils gmock_main gtest)

  add_test(
    NAME cpputils_unittest_miscs
    WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
    COMMAND "${EXECUTABLE_OUTPUT_PATH}/cpputils_unittest_miscs")

endif(CPPUTILS_BUILD_TESTS)
