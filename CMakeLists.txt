cmake_minimum_required(VERSION 3.18)
project(toonify)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall")

# Write targets to bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 14)

set(GIT_VERSION_STRING "git version 2.20.1 (Apple Git-117)")

#
# Download and unpack googletest at configure time
#
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
		"${CMAKE_BINARY_DIR}/googletest-build")

# Enable Test
enable_testing()

# Logging via spdlog
find_package(spdlog REQUIRED)

# Qt required for UX elements
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_PREFIX_PATH "/Users/dave/Qt/5.15.2/clang_64/lib/cmake/Qt5")
list(APPEND CMAKE_PREFIX_PATH "/Users/dave/Qt/5.15.2/clang_64/lib/cmake/Qt5Widgets")
list(APPEND CMAKE_PREFIX_PATH "/Users/dave/Qt/5.15.2/clang_64/lib/cmake/Qt5OpenGL")
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)

# OpenGL
IF (APPLE)
	INCLUDE_DIRECTORIES(/System/Library/Frameworks)
	INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIR})
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)
	FIND_LIBRARY(GLUT_LIBRARY GLUT)
	FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
	MARK_AS_ADVANCED(COCOA_LIBRARY
			GLUT_LIBRARY
			OpenGL_LIBRARY)
	SET(EXTRA_LIBS ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY})
ENDIF (APPLE)
add_definitions(-DGL_SILENCE_DEPRECATION)


add_executable(
		toonify
		main.cpp
		toonify_window.h toonify_window.cpp toonify_window.ui
		toonify_widget.h toonify_widget.cpp
)
target_include_directories(
		toonify
		PRIVATE
		${CMAKE_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/third_party/include
)
target_link_libraries(
		toonify
		PUBLIC
		Qt5::Widgets
		Qt5::OpenGL
		${OPENGL_LIBRARIES}
)
