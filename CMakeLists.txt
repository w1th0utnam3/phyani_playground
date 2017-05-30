cmake_minimum_required(VERSION 3.4)
project("pyhani_playground")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif()

option (USE_GLAD 
        "Use Glad as OpenGL loader" ON) 
		
if (USE_GLAD)
  set (PHYANI_INCLUDES ${PHYANI_INCLUDES} "${CMAKE_CURRENT_LIST_DIR}/lib/glad/include")
  add_subdirectory (lib/glad)
  set (PHYANI_LIBS ${PHYANI_LIBS} glad)
endif (USE_GLAD)

MESSAGE( STATUS ${PHYANI_LIBS} )
add_subdirectory (submodules)
MESSAGE( STATUS ${PHYANI_LIBS} )

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_subdirectory (src)