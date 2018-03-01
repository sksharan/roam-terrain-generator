set(SOURCE_FILES
    BTT.h BTT.cpp
    Camera.h Camera.cpp
    Globals.h Globals.cpp
    GrassPatch.h GrassPatch.cpp
    GridTerrain.h GridTerrain.cpp
    KeyHandler.h KeyHandler.cpp
    LandscapeManager.h LandscapeManager.cpp
    Main.cpp
    MouseHandler.h MouseHandler.cpp
    Object.h Object.cpp
    Renderer.h Renderer.cpp
    ROAMTerrain.h ROAMTerrain.cpp
    Utils.h Utils.cpp
)

# Set up shader and texture file tabs in Visual Studio
# https://cmake.org/cmake/help/v3.8/command/source_group.html
set(SHADER_FOLDER "shaders")
set(SHADER_FILES
    "${SHADER_FOLDER}/grass.frag"
    "${SHADER_FOLDER}/grass.vert"
    "${SHADER_FOLDER}/roamterrain.frag"
    "${SHADER_FOLDER}/roamterrain.vert"
)
source_group(${SHADER_FOLDER} FILES ${SHADER_FILES})
set(TEXTURE_FOLDER "textures")
set(TEXTURE_FILES
    "${TEXTURE_FOLDER}/dirt.jpg"
    "${TEXTURE_FOLDER}/grass_blades.png"
    "${TEXTURE_FOLDER}/grass.jpg"
    "${TEXTURE_FOLDER}/rock.jpg"
)
source_group(${TEXTURE_FOLDER} FILES ${TEXTURE_FILES})

add_executable(TerrainGenerator ${SOURCE_FILES} ${SHADER_FILES} ${TEXTURE_FILES})

# Allow Visual Studio to locate the shader and texture resource files
# https://stackoverflow.com/questions/23950887/does-cmake-offer-a-method-to-set-the-working-directory-for-a-given-build-system
set_target_properties(TerrainGenerator PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Copy libraries to the same folder as the executable
# https://stackoverflow.com/questions/10671916/how-to-copy-dll-files-into-the-same-folder-as-the-executable-using-cmake
function(copyLibraries libs)
    # Assume all files to copy are in the same directory
    list(GET libs 0 firstLib)
    get_filename_component(libDir ${firstLib} DIRECTORY)
    add_custom_command(TARGET TerrainGenerator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${libDir}"
            $<TARGET_FILE_DIR:TerrainGenerator>)
endfunction()

find_package(OpenGL REQUIRED)
target_include_directories(TerrainGenerator PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(TerrainGenerator ${OPENGL_LIBRARIES})

find_package(OpenMP REQUIRED)
# https://stackoverflow.com/questions/12399422/how-to-set-linker-flags-for-openmp-in-cmakes-try-compile-function
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

find_package(SDL2 REQUIRED)
target_include_directories(TerrainGenerator PRIVATE ${SDL2_INCLUDE_DIR})
target_link_libraries(TerrainGenerator ${SDL2_LIBRARY})
copyLibraries(${SDL2_LIBRARY})

find_package(GLEW REQUIRED)
target_include_directories(TerrainGenerator PRIVATE ${GLEW_INCLUDE_DIRS})
target_link_libraries(TerrainGenerator ${GLEW_LIBRARIES})
copyLibraries(${GLEW_LIBRARIES})

find_package(GLM REQUIRED)
target_include_directories(TerrainGenerator PRIVATE ${GLM_INCLUDE_DIRS})

find_package(LibNoise REQUIRED)
target_include_directories(TerrainGenerator PRIVATE ${LIBNOISE_INCLUDE_DIR})
target_link_libraries(TerrainGenerator ${LIBNOISE_LIBRARIES})
copyLibraries(${LIBNOISE_LIBRARIES})

find_package(SOIL REQUIRED)
target_include_directories(TerrainGenerator PRIVATE ${SOIL_INCLUDE_DIRS})
target_link_libraries(TerrainGenerator ${SOIL_LIBRARIES})
copyLibraries(${SOIL_LIBRARIES})
