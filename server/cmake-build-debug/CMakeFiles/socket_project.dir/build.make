# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.8

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "E:\Program Files\Jetbrains\apps\CLion\ch-0\172.4343.16\bin\cmake\bin\cmake.exe"

# The command to remove a file.
RM = "E:\Program Files\Jetbrains\apps\CLion\ch-0\172.4343.16\bin\cmake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = F:\prog_pack\server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = F:\prog_pack\server\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/socket_project.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/socket_project.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/socket_project.dir/flags.make

CMakeFiles/socket_project.dir/server.cpp.obj: CMakeFiles/socket_project.dir/flags.make
CMakeFiles/socket_project.dir/server.cpp.obj: ../server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=F:\prog_pack\server\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/socket_project.dir/server.cpp.obj"
	C:\bin\mingw\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\socket_project.dir\server.cpp.obj -c F:\prog_pack\server\server.cpp

CMakeFiles/socket_project.dir/server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/socket_project.dir/server.cpp.i"
	C:\bin\mingw\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E F:\prog_pack\server\server.cpp > CMakeFiles\socket_project.dir\server.cpp.i

CMakeFiles/socket_project.dir/server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/socket_project.dir/server.cpp.s"
	C:\bin\mingw\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S F:\prog_pack\server\server.cpp -o CMakeFiles\socket_project.dir\server.cpp.s

CMakeFiles/socket_project.dir/server.cpp.obj.requires:

.PHONY : CMakeFiles/socket_project.dir/server.cpp.obj.requires

CMakeFiles/socket_project.dir/server.cpp.obj.provides: CMakeFiles/socket_project.dir/server.cpp.obj.requires
	$(MAKE) -f CMakeFiles\socket_project.dir\build.make CMakeFiles/socket_project.dir/server.cpp.obj.provides.build
.PHONY : CMakeFiles/socket_project.dir/server.cpp.obj.provides

CMakeFiles/socket_project.dir/server.cpp.obj.provides.build: CMakeFiles/socket_project.dir/server.cpp.obj


# Object files for target socket_project
socket_project_OBJECTS = \
"CMakeFiles/socket_project.dir/server.cpp.obj"

# External object files for target socket_project
socket_project_EXTERNAL_OBJECTS =

socket_project.exe: CMakeFiles/socket_project.dir/server.cpp.obj
socket_project.exe: CMakeFiles/socket_project.dir/build.make
socket_project.exe: CMakeFiles/socket_project.dir/linklibs.rsp
socket_project.exe: CMakeFiles/socket_project.dir/objects1.rsp
socket_project.exe: CMakeFiles/socket_project.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=F:\prog_pack\server\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable socket_project.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\socket_project.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/socket_project.dir/build: socket_project.exe

.PHONY : CMakeFiles/socket_project.dir/build

CMakeFiles/socket_project.dir/requires: CMakeFiles/socket_project.dir/server.cpp.obj.requires

.PHONY : CMakeFiles/socket_project.dir/requires

CMakeFiles/socket_project.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\socket_project.dir\cmake_clean.cmake
.PHONY : CMakeFiles/socket_project.dir/clean

CMakeFiles/socket_project.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" F:\prog_pack\server F:\prog_pack\server F:\prog_pack\server\cmake-build-debug F:\prog_pack\server\cmake-build-debug F:\prog_pack\server\cmake-build-debug\CMakeFiles\socket_project.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/socket_project.dir/depend

