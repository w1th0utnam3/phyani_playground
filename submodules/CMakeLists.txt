# Eigen math library
set (SUBMODULE_INCLUDES ${SUBMODULE_INCLUDES} "${CMAKE_CURRENT_LIST_DIR}/eigen")
# EnTT entity component system
set (SUBMODULE_INCLUDES ${SUBMODULE_INCLUDES} "${CMAKE_CURRENT_LIST_DIR}/entt/src")
# mpark variant
set (SUBMODULE_INCLUDES ${SUBMODULE_INCLUDES} "${CMAKE_CURRENT_LIST_DIR}/variant/include")
# OpenGL Math library
set (SUBMODULE_INCLUDES ${SUBMODULE_INCLUDES} "${CMAKE_CURRENT_LIST_DIR}/glm")

# tinyobjloader
file(GLOB TINYOBJLOADER_SOURCES "tinyobjloader/tiny_obj_loader.cc")
file(GLOB TINYOBJLOADER_HEADERS "tinyobjloader/tiny_obj_loader.h")
set (SUBMODULE_INCLUDES ${SUBMODULE_INCLUDES} "${CMAKE_CURRENT_LIST_DIR}/tinyobjloader")
add_library(tinyobjloader ${TINYOBJLOADER_SOURCES} ${TINYOBJLOADER_HEADERS})

# dear imgui
file(GLOB IMGUI_SOURCES "imgui/*.cpp")
file(GLOB IMGUI_HEADERS "imgui/*.h")
set (SUBMODULE_INCLUDES ${SUBMODULE_INCLUDES} "${CMAKE_CURRENT_LIST_DIR}/imgui")
add_library(imgui ${IMGUI_SOURCES} ${IMGUI_HEADERS})

# ImGuizmo
file(GLOB IMGUIZMO_SOURCES "imguizmo/*.cpp")
file(GLOB IMGUIZMO_HEADERS "imguizmo/*.h")
set (SUBMODULE_INCLUDES ${SUBMODULE_INCLUDES} "${CMAKE_CURRENT_LIST_DIR}/imguizmo")
include_directories ("${CMAKE_CURRENT_LIST_DIR}/imgui")
add_library(imguizmo ${IMGUIZMO_SOURCES} ${IMGUIZMO_HEADERS})

# Noname tools
set (SUBMODULE_INCLUDES ${SUBMODULE_INCLUDES} "${CMAKE_CURRENT_LIST_DIR}/noname_tools/src")

# Disable generation of docs, tests and examples for glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# Add glfw project
add_subdirectory (glfw)

set (PHYANI_INCLUDES ${PHYANI_INCLUDES} ${SUBMODULE_INCLUDES} PARENT_SCOPE)
# Add the libs from the submodules folder to the global 
set (PHYANI_LIBS ${PHYANI_LIBS} glfw imgui imguizmo PARENT_SCOPE)
