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
include examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/depend.make

# Include the progress variables for this target.
include examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/progress.make

# Include the compile flags for this target's objects.
include examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/flags.make

examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o: examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/flags.make
examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o: examples/04_Isend_Irecv/fenix/fenix_stencil_1D.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/evalen/public/Fenix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o"
	cd /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix && /home/knteran/home-local/mpi-ulfm/bin/mpicc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o   -c /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix/fenix_stencil_1D.c

examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.i"
	cd /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix && /home/knteran/home-local/mpi-ulfm/bin/mpicc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix/fenix_stencil_1D.c > CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.i

examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.s"
	cd /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix && /home/knteran/home-local/mpi-ulfm/bin/mpicc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix/fenix_stencil_1D.c -o CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.s

examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o.requires:

.PHONY : examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o.requires

examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o.provides: examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o.requires
	$(MAKE) -f examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/build.make examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o.provides.build
.PHONY : examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o.provides

examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o.provides.build: examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o


# Object files for target fenix_stencil_1D
fenix_stencil_1D_OBJECTS = \
"CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o"

# External object files for target fenix_stencil_1D
fenix_stencil_1D_EXTERNAL_OBJECTS =

examples/04_Isend_Irecv/fenix/fenix_stencil_1D: examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o
examples/04_Isend_Irecv/fenix/fenix_stencil_1D: examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/build.make
examples/04_Isend_Irecv/fenix/fenix_stencil_1D: src/libfenix.a
examples/04_Isend_Irecv/fenix/fenix_stencil_1D: examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/evalen/public/Fenix/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable fenix_stencil_1D"
	cd /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fenix_stencil_1D.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/build: examples/04_Isend_Irecv/fenix/fenix_stencil_1D

.PHONY : examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/build

examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/requires: examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/fenix_stencil_1D.c.o.requires

.PHONY : examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/requires

examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/clean:
	cd /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix && $(CMAKE_COMMAND) -P CMakeFiles/fenix_stencil_1D.dir/cmake_clean.cmake
.PHONY : examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/clean

examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/depend:
	cd /home/evalen/public/Fenix && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/evalen/public/Fenix /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix /home/evalen/public/Fenix /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix /home/evalen/public/Fenix/examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/04_Isend_Irecv/fenix/CMakeFiles/fenix_stencil_1D.dir/depend
