cmake_minimum_required(VERSION 3.9)
project(sim CXX)

# Set to ON when viewing full build output is desired
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(SOURCE_FILES
		src/main.cpp
		src/instruction_metadata.cpp
		src/instruction_metadata.h
		src/instruction.cpp
		src/instruction.h
		src/pipeline_names.h
		src/pipeline.cpp
		src/pipeline.h
		src/simulator.cpp
		src/simulator.h)

add_executable(sim ${SOURCE_FILES})
set_target_properties(sim PROPERTIES
		CXX_STANDARD 11
		CXX_EXTENSIONS OFF)
target_compile_features(sim PUBLIC cxx_std_11)

if (NOT MSVC)
	set(PROJECT_COMPILE_FLAGS
			-Wall
			-Wextra
			-pedantic
			-D_FORTIFY_SOURCE=2
			-D_GLIBCXX_ASSERTIONS
			-fasynchronous-unwind-tables
			-fexceptions
			-fpic
			-fstack-protector-all
			-fstack-protector-strong
			-grecord-gcc-switches
			-pipe
			-Werror=format-security
			-fpie)

	if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
		list(APPEND PROJECT_COMPILE_FLAGS
				-shared
				-Wl,-pie
				-Wl,-z,defs
				-Wl,-z,now
				-Wl,-z,relro)
	endif(CMAKE_CXX_COMPILER_ID MATCHES GNU)

	set(PROJECT_DEBUG_FLAGS
			${PROJECT_COMPILE_FLAGS}
			-g
			-O0)

	set(PROJECT_RELEASE_FLAGS
			${PROJECT_COMPILE_FLAGS}
			-O3)

	target_compile_options(sim
			PUBLIC $<$<CONFIG:Debug>:${PROJECT_DEBUG_FLAGS}>)

	target_compile_options(sim
			PUBLIC $<$<CONFIG:Release>:${PROJECT_RELEASE_FLAGS}>)
endif(NOT MSVC)

add_executable(scope validation/tool/main.cc validation/tool/printline.h)
set_target_properties(scope PROPERTIES
		CXX_STANDARD 11
		CXX_EXTENSIONS OFF)
target_compile_features(scope PUBLIC cxx_std_11)

if (NOT MSVC)
	set(SCOPE_COMPILE_FLAGS
			-Wall
			-Wextra
			-pedantic
			-D_FORTIFY_SOURCE=2
			-D_GLIBCXX_ASSERTIONS
			-fasynchronous-unwind-tables
			-fexceptions
			-fpic
			-fstack-protector-all
			-fstack-protector-strong
			-grecord-gcc-switches
			-pipe
			-fpie)

	if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
		list(APPEND SCOPE_COMPILE_FLAGS
				-shared
				-Wl,-pie
				-Wl,-z,defs
				-Wl,-z,now
				-Wl,-z,relro)
	endif(CMAKE_CXX_COMPILER_ID MATCHES GNU)

	set(SCOPE_DEBUG_FLAGS
			${SCOPE_COMPILE_FLAGS}
			-g
			-O0)

	set(SCOPE_RELEASE_FLAGS
			${SCOPE_COMPILE_FLAGS}
			-O3)

	target_compile_options(scope
			PUBLIC $<$<CONFIG:Debug>:${SCOPE_DEBUG_FLAGS}>)

	target_compile_options(scope
			PUBLIC $<$<CONFIG:Release>:${SCOPE_RELEASE_FLAGS}>)
endif(NOT MSVC)
