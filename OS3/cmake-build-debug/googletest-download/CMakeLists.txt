cmake_minimum_required(VERSION 2.8.2)

project(googletest-download NONE)

include(ExternalProject)
ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG           master
  SOURCE_DIR        "/cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/cmake-build-debug/googletest-src"
  BINARY_DIR        "/cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/cmake-build-debug/googletest-build"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
