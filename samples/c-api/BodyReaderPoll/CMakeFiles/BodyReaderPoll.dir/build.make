# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jackfan108/AstraSDK/samples

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jackfan108/AstraSDK/samples

# Include any dependencies generated for this target.
include c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/depend.make

# Include the progress variables for this target.
include c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/progress.make

# Include the compile flags for this target's objects.
include c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/flags.make

c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o: c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/flags.make
c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o: c-api/BodyReaderPoll/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jackfan108/AstraSDK/samples/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o"
	cd /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/BodyReaderPoll.dir/main.c.o   -c /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll/main.c

c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/BodyReaderPoll.dir/main.c.i"
	cd /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll/main.c > CMakeFiles/BodyReaderPoll.dir/main.c.i

c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/BodyReaderPoll.dir/main.c.s"
	cd /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll/main.c -o CMakeFiles/BodyReaderPoll.dir/main.c.s

c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o.requires:

.PHONY : c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o.requires

c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o.provides: c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o.requires
	$(MAKE) -f c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/build.make c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o.provides.build
.PHONY : c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o.provides

c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o.provides.build: c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o


# Object files for target BodyReaderPoll
BodyReaderPoll_OBJECTS = \
"CMakeFiles/BodyReaderPoll.dir/main.c.o"

# External object files for target BodyReaderPoll
BodyReaderPoll_EXTERNAL_OBJECTS =

bin/BodyReaderPoll: c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o
bin/BodyReaderPoll: c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/build.make
bin/BodyReaderPoll: /home/jackfan108/AstraSDK/lib/libastra_core.so
bin/BodyReaderPoll: /home/jackfan108/AstraSDK/lib/libastra_core_api.so
bin/BodyReaderPoll: /home/jackfan108/AstraSDK/lib/libastra.so
bin/BodyReaderPoll: c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jackfan108/AstraSDK/samples/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../../bin/BodyReaderPoll"
	cd /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/BodyReaderPoll.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/build: bin/BodyReaderPoll

.PHONY : c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/build

c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/requires: c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/main.c.o.requires

.PHONY : c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/requires

c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/clean:
	cd /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll && $(CMAKE_COMMAND) -P CMakeFiles/BodyReaderPoll.dir/cmake_clean.cmake
.PHONY : c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/clean

c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/depend:
	cd /home/jackfan108/AstraSDK/samples && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jackfan108/AstraSDK/samples /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll /home/jackfan108/AstraSDK/samples /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll /home/jackfan108/AstraSDK/samples/c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : c-api/BodyReaderPoll/CMakeFiles/BodyReaderPoll.dir/depend

