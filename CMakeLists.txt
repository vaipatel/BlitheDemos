cmake_minimum_required(VERSION 3.15)

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
find_package(GLFW3 QUIET)

if(NOT GLFW3_FOUND) # GLFW not found, use the fallback
    message(STATUS "GLFW3 not found with find_package. Using precompiled MSVC binaries as a fallback.")

    # Only MSVC fallback supported for now
    if (MSVC)
        message(STATUS "MSCV version is ${MSVC_VERSION}")
    else()
        message(FATAL_ERROR "MSVC compiler not detected. Only precompiled MSVC binaries supported as fallback for GLFW3")
    endif()

	set(GLFW3_FALLBACK_DIR "${PROJECT_SOURCE_DIR}/../BlitheDemosVendors/glfw/glfw-3.3.8.bin.WIN64" CACHE PATH "Fallback User Path to GLFW3")

    # Include directory
    include_directories(${GLFW3_FALLBACK_DIR}/include)

    # Select the correct library path based on MSVC version
    if(MSVC_VERSION EQUAL 1900)
        set(GLFW_LIB_PATH "${GLFW3_FALLBACK_DIR}/lib-vc2015")
    elseif(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS_EQUAL 1919)
        set(GLFW_LIB_PATH "${GLFW3_FALLBACK_DIR}/lib-vc2017")
    elseif(MSVC_VERSION EQUAL 1920)
        set(GLFW_LIB_PATH "${GLFW3_FALLBACK_DIR}/lib-vc2019")
    else()
        message(FATAL_ERROR "Unsupported MSVC version for GLFW fallback.")
    endif()

    # Assuming we're targeting a specific configuration (e.g., Release)
    set(GLFW_LIBRARY "${GLFW_LIB_PATH}/glfw3.lib")

    # For copying the DLL in Windows
    set(GLFW_DLL "${GLFW_LIB_PATH}/glfw3.dll")
else()
    # If found, use the found package's configuration
    set(GLFW_LIBRARY glfw)
endif()


# Glad
# ----
set(GLAD_DIR "${PROJECT_SOURCE_DIR}/../BlitheDemosVendors/glad" CACHE PATH "User Path to GLAD")
include_directories(${GLAD_DIR}/include)

# ImGui
# -----
set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/../BlitheDemosVendors/imgui" CACHE PATH "User Path to ImGui")
file(GLOB IMGUI_SOURCES 
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)
file(GLOB IMGUI_HEADERS
    ${IMGUI_DIR}/*.h
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
    ${IMGUI_DIR}/backends/imgui_impl_opengl3_loader.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.h
)

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(imgui PUBLIC ${IMGUI_DIR})
target_include_directories(imgui PUBLIC ${IMGUI_DIR}/backends)

# stb
# ---
set(STB_DIR "${PROJECT_SOURCE_DIR}/../BlitheDemosVendors/stb" CACHE PATH "User Path to stb")
include_directories(${STB_DIR})

# GLM
# ---
find_package(glm QUIET)

# My sources and headers
# ----------------------
set(BLITHE_DEMOS_SOURCES
    ${PROJECT_SOURCE_DIR}/App/main.cpp
    ${PROJECT_SOURCE_DIR}/App/BlitheDemosApp.cpp
    ${PROJECT_SOURCE_DIR}/App/Demo/CubeDemo.cpp
    ${PROJECT_SOURCE_DIR}/App/Demo/TriangleDemo.cpp
    ${PROJECT_SOURCE_DIR}/App/Geometry/Camera.cpp
    ${PROJECT_SOURCE_DIR}/App/Geometry/YawPitchCameraDecorator.cpp
    ${PROJECT_SOURCE_DIR}/App/Geometry/GeomHelpers.cpp
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
    ${PROJECT_SOURCE_DIR}/App/Demo/TriangleDemo.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/Camera.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/CameraDecorator.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/YawPitchCameraDecorator.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/GeomHelpers.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/Mesh.h
    ${PROJECT_SOURCE_DIR}/App/Geometry/Tri.h
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
file(GLOB BLITHE_DEMOS_ASSETS 
    "${PROJECT_SOURCE_DIR}/Assets/*.jpg"
)

# Add main executable
# -------------------
add_executable(BlitheDemos ${BLITHE_DEMOS_SOURCES} ${BLITHE_DEMOS_HEADERS} ${GLAD_DIR}/src/glad.c)

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

# Add shaders to the executable so they show up in IDEs
# -----------------------------------------------------
target_sources(BlitheDemos PRIVATE ${BLITHE_DEMOS_SHADERS})

if(NOT glm_FOUND) # If GLM is not found, use the bundled version and set up include directories
    message(STATUS "GLM not found on the system. Using the bundled version.")
	
    set(GLM_FALLBACK_DIR "${PROJECT_SOURCE_DIR}/../BlitheDemosVendors/glm-0.9.9.8" CACHE PATH "Fallback User Path to glm")

    target_include_directories(BlitheDemos PRIVATE ${GLM_FALLBACK_DIR})
endif()

# Link against GLFW
# -----------------
target_link_libraries(BlitheDemos PRIVATE ${GLFW_LIBRARY} imgui)
target_link_libraries(BlitheDemos PRIVATE OpenGL::GL)

# Copy the GLFW DLL to the output directory if using the fallback
if(NOT GLFW3_FOUND AND EXISTS "${GLFW_DLL}")
    add_custom_command(TARGET BlitheDemos POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_DLL}"
        $<TARGET_FILE_DIR:BlitheDemos>)
endif()

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
    add_custom_command(TARGET BlitheDemos POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${ASSET}
                $<TARGET_FILE_DIR:BlitheDemos>/Assets/${ASSET_NAME})
endforeach()
