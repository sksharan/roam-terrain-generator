cmake_minimum_required(VERSION 3.8)
project(TerrainGenerator)

# Directory containing additional FindXXXX.cmake files
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# For FindSDL2.cmake
set(SDL2_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2-2.0.7")

# For FindGLEW.cmake
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/glew-2.1.0")

# For FindGLM.cmake
set(GLM_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glm")

# For FindLibNoise.cmake
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/libnoise")

# For FindSOIL.cmake
set(SOIL_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/external/Simple OpenGL Image Library/src")
# The path to the library after building SOIL
set(SOIL_LIBDIR "${CMAKE_CURRENT_SOURCE_DIR}/external/Simple OpenGL Image Library/projects/VC9/Debug")

add_subdirectory(TerrainGenerator)
