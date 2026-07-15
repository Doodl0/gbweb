cmake_minimum_required(VERSION 3.16)
project(gbweb)

# Set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Create your game executable target as usual
add_executable(gbweb EXCLUDE_FROM_ALL src/gbweb.c)

# on Web targets, we need CMake to generate a HTML webpage.
if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
  target_compile_options(gbweb PRIVATE -sUSE_SDL=2)
  target_link_options(gbweb PRIVATE -sUSE_SDL=2)
endif()
