cmake_minimum_required(VERSION 3.15)

project(ImEngine)

set (CMAKE_CXX_STANDARD 11)

if(MSVC)
    # If I don't set the below to MD, I get LNK4217 warnings and LNK2019 errors with glfw3.lib
	# likely because of MSVCRT library being MD.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
find_package(GLFW3 QUIET)

if(NOT GLFW3_FOUND) # GLFW not found, use the fallback
    message(STATUS "GLFW3 not found with find_package. Using precompiled binaries as a fallback.")

	set(GLFW3_FALLBACK_DIR "${PROJECT_SOURCE_DIR}/../ImEngineVendors/glfw/glfw-3.3.8.bin.WIN64" CACHE PATH "Fallback User Path to GLFW3")

    # Include directory
    include_directories(${GLFW3_FALLBACK_DIR}/include)

    # Select the correct library path based on MSVC version
    if(MSVC_VERSION EQUAL 1900)
        set(GLFW_LIB_PATH "${GLFW3_FALLBACK_DIR}/lib-vc2015")
    elseif(MSVC_VERSION EQUAL 1910)
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
set(GLAD_DIR "${PROJECT_SOURCE_DIR}/../ImEngineVendors/glad" CACHE PATH "User Path to GLAD")
include_directories(${GLAD_DIR}/include)

# ImGui
set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/../ImEngineVendors/imgui" CACHE PATH "User Path to ImGui")
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
set(STB_DIR "${PROJECT_SOURCE_DIR}/../ImEngineVendors/stb" CACHE PATH "User Path to stb")
include_directories(${STB_DIR})

# GLM
find_package(glm QUIET)

# Include directories for header files
include_directories(include)

add_executable(ImEngine app/main.cpp app/ImEngineApp.cpp ${GLAD_DIR}/src/glad.c)

if(NOT glm_FOUND) # If GLM is not found, use the bundled version and set up include directories
    message(STATUS "GLM not found on the system. Using the bundled version.")
	
    set(GLM_FALLBACK_DIR "${PROJECT_SOURCE_DIR}/../ImEngineVendors/glm-0.9.9.8" CACHE PATH "Fallback User Path to glm")

    target_include_directories(ImEngine PRIVATE ${GLM_FALLBACK_DIR})
endif()

# Link against GLFW
target_link_libraries(ImEngine PRIVATE ${GLFW_LIBRARY} imgui)
target_link_libraries(ImEngine PRIVATE OpenGL::GL)

# Copy the GLFW DLL to the output directory if using the fallback
if(NOT GLFW3_FOUND AND EXISTS "${GLFW_DLL}")
    add_custom_command(TARGET ImEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_DLL}"
        $<TARGET_FILE_DIR:ImEngine>)
endif()
