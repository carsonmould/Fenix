# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

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
CMAKE_COMMAND = /spack/c7.3/base/spack/opt/spack/linux-rhel7-x86_64/gcc-4.8.5/cmake-3.6.1-zhy253bsepzz2fyst5nmlotw3eemciol/bin/cmake

# The command to remove a file.
RM = /spack/c7.3/base/spack/opt/spack/linux-rhel7-x86_64/gcc-4.8.5/cmake-3.6.1-zhy253bsepzz2fyst5nmlotw3eemciol/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/evalen/public/Fenix

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/evalen/public/Fenix

# Include any dependencies generated for this target.
include test/online/CMakeFiles/online.dir/depend.make

# Include the progress variables for this target.
include test/online/CMakeFiles/online.dir/progress.make

# Include the compile flags for this target's objects.
include test/online/CMakeFiles/online.dir/flags.make

test/online/CMakeFiles/online.dir/online.c.o: test/online/CMakeFiles/online.dir/flags.make
test/online/CMakeFiles/online.dir/online.c.o: test/online/online.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/evalen/public/Fenix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object test/online/CMakeFiles/online.dir/online.c.o"
	cd /home/evalen/public/Fenix/test/online && /home/knteran/home-local/mpi-ulfm/bin/mpicc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/online.dir/online.c.o   -c /home/evalen/public/Fenix/test/online/online.c

test/online/CMakeFiles/online.dir/online.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/online.dir/online.c.i"
	cd /home/evalen/public/Fenix/test/online && /home/knteran/home-local/mpi-ulfm/bin/mpicc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/evalen/public/Fenix/test/online/online.c > CMakeFiles/online.dir/online.c.i

test/online/CMakeFiles/online.dir/online.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/online.dir/online.c.s"
	cd /home/evalen/public/Fenix/test/online && /home/knteran/home-local/mpi-ulfm/bin/mpicc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/evalen/public/Fenix/test/online/online.c -o CMakeFiles/online.dir/online.c.s

test/online/CMakeFiles/online.dir/online.c.o.requires:

.PHONY : test/online/CMakeFiles/online.dir/online.c.o.requires

test/online/CMakeFiles/online.dir/online.c.o.provides: test/online/CMakeFiles/online.dir/online.c.o.requires
	$(MAKE) -f test/online/CMakeFiles/online.dir/build.make test/online/CMakeFiles/online.dir/online.c.o.provides.build
.PHONY : test/online/CMakeFiles/online.dir/online.c.o.provides

test/online/CMakeFiles/online.dir/online.c.o.provides.build: test/online/CMakeFiles/online.dir/online.c.o


# Object files for target online
online_OBJECTS = \
"CMakeFiles/online.dir/online.c.o"

# External object files for target online
online_EXTERNAL_OBJECTS =

test/online/online: test/online/CMakeFiles/online.dir/online.c.o
test/online/online: test/online/CMakeFiles/online.dir/build.make
test/online/online: test/online/CMakeFiles/online.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/evalen/public/Fenix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable online"
	cd /home/evalen/public/Fenix/test/online && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/online.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/online/CMakeFiles/online.dir/build: test/online/online

.PHONY : test/online/CMakeFiles/online.dir/build

test/online/CMakeFiles/online.dir/requires: test/online/CMakeFiles/online.dir/online.c.o.requires

.PHONY : test/online/CMakeFiles/online.dir/requires

test/online/CMakeFiles/online.dir/clean:
	cd /home/evalen/public/Fenix/test/online && $(CMAKE_COMMAND) -P CMakeFiles/online.dir/cmake_clean.cmake
.PHONY : test/online/CMakeFiles/online.dir/clean

test/online/CMakeFiles/online.dir/depend:
	cd /home/evalen/public/Fenix && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/evalen/public/Fenix /home/evalen/public/Fenix/test/online /home/evalen/public/Fenix /home/evalen/public/Fenix/test/online /home/evalen/public/Fenix/test/online/CMakeFiles/online.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/online/CMakeFiles/online.dir/depend

