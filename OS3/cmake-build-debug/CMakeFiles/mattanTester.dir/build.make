# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /cygdrive/c/Users/owner/.CLion2019.3/system/cygwin_cmake/bin/cmake.exe

# The command to remove a file.
RM = /cygdrive/c/Users/owner/.CLion2019.3/system/cygwin_cmake/bin/cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/mattanTester.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/mattanTester.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mattanTester.dir/flags.make

CMakeFiles/mattanTester.dir/SampleClient.cpp.o: CMakeFiles/mattanTester.dir/flags.make
CMakeFiles/mattanTester.dir/SampleClient.cpp.o: ../SampleClient.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/mattanTester.dir/SampleClient.cpp.o"
	/usr/bin/c++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/mattanTester.dir/SampleClient.cpp.o -c /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/SampleClient.cpp

CMakeFiles/mattanTester.dir/SampleClient.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mattanTester.dir/SampleClient.cpp.i"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/SampleClient.cpp > CMakeFiles/mattanTester.dir/SampleClient.cpp.i

CMakeFiles/mattanTester.dir/SampleClient.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mattanTester.dir/SampleClient.cpp.s"
	/usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/SampleClient.cpp -o CMakeFiles/mattanTester.dir/SampleClient.cpp.s

# Object files for target mattanTester
mattanTester_OBJECTS = \
"CMakeFiles/mattanTester.dir/SampleClient.cpp.o"

# External object files for target mattanTester
mattanTester_EXTERNAL_OBJECTS =

mattanTester.exe: CMakeFiles/mattanTester.dir/SampleClient.cpp.o
mattanTester.exe: CMakeFiles/mattanTester.dir/build.make
mattanTester.exe: lib/libgtest_maind.a
mattanTester.exe: lib/libgtestd.a
mattanTester.exe: CMakeFiles/mattanTester.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable mattanTester.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mattanTester.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mattanTester.dir/build: mattanTester.exe

.PHONY : CMakeFiles/mattanTester.dir/build

CMakeFiles/mattanTester.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mattanTester.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mattanTester.dir/clean

CMakeFiles/mattanTester.dir/depend:
	cd /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3 /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3 /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/cmake-build-debug /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/cmake-build-debug /cygdrive/c/Users/owner/CLionProjects/OS3.2/OS3/cmake-build-debug/CMakeFiles/mattanTester.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mattanTester.dir/depend

