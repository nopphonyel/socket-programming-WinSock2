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
CMAKE_COMMAND = "D:\Program Files\JetBrains\apps\CLion\ch-0\172.4343.16\bin\cmake\bin\cmake.exe"

# The command to remove a file.
RM = "D:\Program Files\JetBrains\apps\CLion\ch-0\172.4343.16\bin\cmake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "D:\Users\nopph\Documents\Network\Socket Project\client"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "D:\Users\nopph\Documents\Network\Socket Project\client\cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/client.dir/flags.make

CMakeFiles/client.dir/main.cpp.obj: CMakeFiles/client.dir/flags.make
CMakeFiles/client.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="D:\Users\nopph\Documents\Network\Socket Project\client\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/client.dir/main.cpp.obj"
	C:\MinGW\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\client.dir\main.cpp.obj -c "D:\Users\nopph\Documents\Network\Socket Project\client\main.cpp"

CMakeFiles/client.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/main.cpp.i"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "D:\Users\nopph\Documents\Network\Socket Project\client\main.cpp" > CMakeFiles\client.dir\main.cpp.i

CMakeFiles/client.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/main.cpp.s"
	C:\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "D:\Users\nopph\Documents\Network\Socket Project\client\main.cpp" -o CMakeFiles\client.dir\main.cpp.s

CMakeFiles/client.dir/main.cpp.obj.requires:

.PHONY : CMakeFiles/client.dir/main.cpp.obj.requires

CMakeFiles/client.dir/main.cpp.obj.provides: CMakeFiles/client.dir/main.cpp.obj.requires
	$(MAKE) -f CMakeFiles\client.dir\build.make CMakeFiles/client.dir/main.cpp.obj.provides.build
.PHONY : CMakeFiles/client.dir/main.cpp.obj.provides

CMakeFiles/client.dir/main.cpp.obj.provides.build: CMakeFiles/client.dir/main.cpp.obj


# Object files for target client
client_OBJECTS = \
"CMakeFiles/client.dir/main.cpp.obj"

# External object files for target client
client_EXTERNAL_OBJECTS =

client.exe: CMakeFiles/client.dir/main.cpp.obj
client.exe: CMakeFiles/client.dir/build.make
client.exe: CMakeFiles/client.dir/linklibs.rsp
client.exe: CMakeFiles/client.dir/objects1.rsp
client.exe: CMakeFiles/client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="D:\Users\nopph\Documents\Network\Socket Project\client\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable client.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\client.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/client.dir/build: client.exe

.PHONY : CMakeFiles/client.dir/build

CMakeFiles/client.dir/requires: CMakeFiles/client.dir/main.cpp.obj.requires

.PHONY : CMakeFiles/client.dir/requires

CMakeFiles/client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\client.dir\cmake_clean.cmake
.PHONY : CMakeFiles/client.dir/clean

CMakeFiles/client.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "D:\Users\nopph\Documents\Network\Socket Project\client" "D:\Users\nopph\Documents\Network\Socket Project\client" "D:\Users\nopph\Documents\Network\Socket Project\client\cmake-build-debug" "D:\Users\nopph\Documents\Network\Socket Project\client\cmake-build-debug" "D:\Users\nopph\Documents\Network\Socket Project\client\cmake-build-debug\CMakeFiles\client.dir\DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/client.dir/depend
