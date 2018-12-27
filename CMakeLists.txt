cmake_minimum_required(VERSION 3.9)
project(kopt VERSION 0.1.0 DESCRIPTION "Argument Parsing Library")

include(GNUInstallDirs)

add_library(kopt SHARED
  src/option_parser.cc
  )
add_library(kopt_static STATIC
  src/option_parser.cc
  )

set(HEADER_FILES
  include/kopt/kopt.h
  include/kopt/argument_option.h
  include/kopt/multi_argument_option.h
  include/kopt/conversion_exception.h
  include/kopt/flag_option.h
  include/kopt/invalid_value_exception.h
  include/kopt/missing_argument_exception.h
  include/kopt/missing_required_option_exception.h
  include/kopt/option.h
  include/kopt/option_parser.h
  include/kopt/unknown_option_exception.h
  include/kopt/no_multi_argument_exception.h
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pedantic -Wall -march=native")

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++17" HAVE_CPP17)
if (NOT HAVE_CPP17)
  message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
endif()

include(CheckFunctionExists)
check_function_exists(getopt_long HAVE_GETOPT_LONG)
if (NOT HAVE_GETOPT_LONG)
  message(FATAL_ERROR "Getopt Long not found.")
endif()

set_target_properties(kopt PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION 0
  PUBLIC_HEADER "${HEADER_FILES}")
set_target_properties(kopt_static PROPERTIES OUTPUT_NAME kopt)

configure_file(kopt.pc.in kopt.pc @ONLY)

target_include_directories(kopt PRIVATE include)
target_include_directories(kopt PRIVATE src)
target_include_directories(kopt_static PRIVATE include)
target_include_directories(kopt_static PRIVATE src)

install(TARGETS kopt LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kopt)
install(TARGETS kopt_static ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${CMAKE_BINARY_DIR}/kopt.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

# Examples
add_executable(flags examples/flags)
target_include_directories(flags PRIVATE include)
target_link_libraries(flags kopt)

add_executable(arguments examples/arguments)
target_include_directories(arguments PRIVATE include)
target_link_libraries(arguments kopt)

add_executable(multi_arguments examples/multi_arguments)
target_include_directories(multi_arguments PRIVATE include)
target_link_libraries(multi_arguments kopt)

add_executable(unparsed examples/unparsed)
target_include_directories(unparsed PRIVATE include)
target_link_libraries(unparsed kopt)
