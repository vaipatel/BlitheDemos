cmake_minimum_required(VERSION 3.22)

# VCPKG
# -----
# Triplets
# Detect the platform and compiler using pre-project() variables
if(WIN32)  # WIN32 is defined on Windows systems
    # Check the generator to infer the compiler
    if(CMAKE_GENERATOR MATCHES "MinGW" OR CMAKE_GENERATOR MATCHES "Ninja")
        # MinGW compiler (or Ninja with MinGW)
        set(VCPKG_DEFAULT_TRIPLET "x64-mingw-dynamic")
    else()
        # Assume MSVC or other Windows compilers
        set(VCPKG_DEFAULT_TRIPLET "x64-windows")
    endif()
elseif(UNIX AND NOT APPLE)  # UNIX is defined on Linux and other UNIX-like systems
    # Linux platform
    set(VCPKG_DEFAULT_TRIPLET "x64-linux")
else()
    message(WARNING "Unsupported platform. Defaulting to x64-windows.")
    set(VCPKG_DEFAULT_TRIPLET "x64-windows")  # Fallback for unsupported platforms
endif()

# Set both target and host triplets to the same value
set(VCPKG_HOST_TRIPLET "${VCPKG_DEFAULT_TRIPLET}" CACHE STRING "Host triplet (same as target)")
set(VCPKG_TARGET_TRIPLET "${VCPKG_DEFAULT_TRIPLET}" CACHE STRING "Default triplet")

# Toolchain
if(DEFINED ENV{VCPKG_ROOT})
    file(TO_CMAKE_PATH $ENV{VCPKG_ROOT} VCPKG_ROOT)
    message(STATUS "VCPKG_ROOT is at ${VCPKG_ROOT}")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
else()
    message(FATAL_ERROR "VCPKG_ROOT is not defined")
endif()

# Project
# -------
project(BlitheDemos)

set (CMAKE_CXX_STANDARD 11)

if(MSVC)
    # If I don't set the below to MD, I get LNK4217 warnings and LNK2019 errors with glfw3.lib
    # likely because of MSVCRT library being MD.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# OpenGL
# ------
find_package(OpenGL REQUIRED)

# GLFW
# ----
find_package(glfw3 CONFIG REQUIRED)

# Glad
# ----
find_package(glad CONFIG REQUIRED)

# ImGui
# -----
find_package(imgui CONFIG REQUIRED)

# stb
# ---
find_package(Stb REQUIRED)

# GLM
# ---
find_package(glm QUIET)

# Assimp
# ------
find_package(assimp CONFIG REQUIRED)

# My sources and headers
# ----------------------
set(BLITHE_DEMOS_SOURCES
    ${PROJECT_SOURCE_DIR}/App/main.cpp
    ${PROJECT_SOURCE_DIR}/App/BlitheDemosApp.cpp
    ${PROJECT_SOURCE_DIR}/App/Demo/CubeDemo.cpp
    ${PROJECT_SOURCE_DIR}/App/Demo/GrassDemo.cpp
    ${PROJECT_SOURCE_DIR}/App/Demo/InstancedCubeDemo.cpp
    ${PROJECT_SOURCE_DIR}/App/Demo/SimpleBSPDemo.cpp
    ${PROJECT_SOURCE_DIR}/App/Demo/TriangleDemo.cpp
    ${PROJECT_SOURCE_DIR}/App/Geometry/ArcBallCameraDecorator.cpp
    ${PROJECT_SOURCE_DIR}/App/Geometry/Camera.cpp
    ${PROJECT_SOURCE_DIR}/App/Geometry/YawPitchCameraDecorator.cpp
    ${PROJECT_SOURCE_DIR}/App/Geometry/GeomHelpers.cpp
    ${PROJECT_SOURCE_DIR}/App/Geometry/MeshView.cpp
    ${PROJECT_SOURCE_DIR}/App/Geometry/TriBSPTree.cpp
    ${PROJECT_SOURCE_DIR}/App/GLWrappers/RenderTarget.cpp
    ${PROJECT_SOURCE_DIR}/App/GLWrappers/ShaderProgram.cpp
    ${PROJECT_SOURCE_DIR}/App/GLWrappers/Texture.cpp
    ${PROJECT_SOURCE_DIR}/App/Objects/MeshObject.cpp
    ${PROJECT_SOURCE_DIR}/App/Objects/TrisObject.cpp
    ${PROJECT_SOURCE_DIR}/App/Utils/BlithePath.cpp
)

set(BLITHE_DEMOS_HEADERS
    ${PROJECT_SOURCE_DIR}/App/BlitheDemosApp.h
    ${PROJECT_SOURCE_DIR}/App/UIData.h
    ${PROJECT_SOURCE_DIR}/App/Demo/DemoInterface.h
    ${PROJECT_SOURCE_DIR}/App/Demo/CubeDemo.h
    ${PROJECT_SOURCE_DIR}/App/Demo/GrassDemo.h
    ${PROJECT_SOURCE_DIR}/App/Demo/InstancedCubeDemo.h
    ${PROJECT_SOURCE_DIR}/App/Demo/SimpleBSPDemo.h
    ${PROJECT_SOURCE_DIR}/App/Demo/TriangleDemo.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/ArcBallCameraDecorator.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/Camera.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/CameraDecorator.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/YawPitchCameraDecorator.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/GeomHelpers.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/Mesh.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/MeshIterator.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/MeshView.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/Tri.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/TriBSPTree.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/Vertex.h
    ${PROJECT_SOURCE_DIR}/App/GLWrappers/RenderTarget.h
    ${PROJECT_SOURCE_DIR}/App/GLWrappers/ShaderProgram.h
    ${PROJECT_SOURCE_DIR}/App/GLWrappers/Texture.h
    ${PROJECT_SOURCE_DIR}/App/GLWrappers/TextureData.h
    ${PROJECT_SOURCE_DIR}/App/Objects/MeshObject.h
    ${PROJECT_SOURCE_DIR}/App/Objects/TrisObject.h
    ${PROJECT_SOURCE_DIR}/App/Utils/BlitheAssert.h
    ${PROJECT_SOURCE_DIR}/App/Utils/BlithePath.h
    ${PROJECT_SOURCE_DIR}/App/Utils/BlitheShared.h
    ${PROJECT_SOURCE_DIR}/App/Utils/BlitheStrUtils.h
)

# My shaders
# ----------
file(GLOB BLITHE_DEMOS_SHADERS 
    "${PROJECT_SOURCE_DIR}/App/Shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/App/Shaders/*.frag"
)

# My assets
# ---------
set(ASSET_PARENT_PATH ${PROJECT_SOURCE_DIR}/Assets)
file(GLOB BLITHE_DEMOS_ASSETS 
    "${ASSET_PARENT_PATH}/*.jpg"
    "${ASSET_PARENT_PATH}/*.obj"
    "${ASSET_PARENT_PATH}/*.stl"
    "${ASSET_PARENT_PATH}/*.ply"
    "${ASSET_PARENT_PATH}/grass yellowing/*.bak"
    "${ASSET_PARENT_PATH}/grass yellowing/*.mtl"
    "${ASSET_PARENT_PATH}/grass yellowing/*.obj"
)

# Add main executable
# -------------------
add_executable(BlitheDemos ${BLITHE_DEMOS_SOURCES} ${BLITHE_DEMOS_HEADERS})

# Organize source files into groups
# ---------------------------------
source_group(TREE "${PROJECT_SOURCE_DIR}/App" PREFIX "Source Files" FILES ${BLITHE_DEMOS_SOURCES})
source_group(TREE "${PROJECT_SOURCE_DIR}/App" PREFIX "Header Files" FILES ${BLITHE_DEMOS_HEADERS})
source_group(TREE "${PROJECT_SOURCE_DIR}/App" PREFIX "Shaders" FILES ${BLITHE_DEMOS_SHADERS})

# Include directories
# -------------------
target_include_directories(BlitheDemos PRIVATE ${PROJECT_SOURCE_DIR}/App)
target_include_directories(BlitheDemos PRIVATE ${PROJECT_SOURCE_DIR}/App/Demo)
target_include_directories(BlitheDemos PRIVATE ${PROJECT_SOURCE_DIR}/App/Geometry)
target_include_directories(BlitheDemos PRIVATE ${PROJECT_SOURCE_DIR}/App/GLWrappers)
target_include_directories(BlitheDemos PRIVATE ${PROJECT_SOURCE_DIR}/App/Objects)
target_include_directories(BlitheDemos PRIVATE ${PROJECT_SOURCE_DIR}/App/Utils)
target_include_directories(BlitheDemos PRIVATE ${PROJECT_SOURCE_DIR}/Extern)
target_include_directories(BlitheDemos PRIVATE ${Stb_INCLUDE_DIR})

# Preprocessor
# ------------
target_compile_definitions(BlitheDemos PUBLIC GLM_ENABLE_EXPERIMENTAL)

# Add shaders to the executable so they show up in IDEs
# -----------------------------------------------------
target_sources(BlitheDemos PRIVATE ${BLITHE_DEMOS_SHADERS})

# Link against GLFW
# -----------------
target_link_libraries(BlitheDemos PRIVATE glm::glm-header-only)
target_link_libraries(BlitheDemos PRIVATE OpenGL::GL)
target_link_libraries(BlitheDemos PRIVATE glad::glad)
target_link_libraries(BlitheDemos PRIVATE glfw)
target_link_libraries(BlitheDemos PRIVATE imgui::imgui)
target_link_libraries(BlitheDemos PRIVATE assimp::assimp)

# Copy shaders over to a "Shaders" directory next to the app
# ----------------------------------------------------------
foreach(SHADER ${BLITHE_DEMOS_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    add_custom_command(TARGET BlitheDemos POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${SHADER}
                $<TARGET_FILE_DIR:BlitheDemos>/Shaders/${SHADER_NAME})
endforeach()

# Copy assets over to a "Assets" directory next to app
# ----------------------------------------------------
foreach(ASSET ${BLITHE_DEMOS_ASSETS})
    get_filename_component(ASSET_NAME ${ASSET} NAME)

    # Get the component of the path upto but not including the ASSET_NAME, call it ASSET_PATH
    cmake_path(GET ASSET PARENT_PATH ASSET_PATH)
    #message(STATUS "Asset Root dir = ${ASSET_PATH}")

    # Get ASSET_PATH relative to ASSET_PARENT_PATH, call it ASSET_REL_PATH
    cmake_path(RELATIVE_PATH ASSET_PATH BASE_DIRECTORY ${ASSET_PARENT_PATH} OUTPUT_VARIABLE ASSET_REL_PATH)
    #message(STATUS "Asset Rel  = ${ASSET_REL_PATH}")

    add_custom_command(TARGET BlitheDemos POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${ASSET}
                $<TARGET_FILE_DIR:BlitheDemos>/Assets/${ASSET_REL_PATH}/${ASSET_NAME})
endforeach()
