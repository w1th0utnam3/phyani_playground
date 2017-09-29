cmake_minimum_required(VERSION 3.4)
project("pyhani_playground")

# Enable C++17 features (e.g. variant)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif()

set (PHYANI_INCLUDES ${PHYANI_INCLUDES} "${CMAKE_CURRENT_LIST_DIR}/lib/glad/include")
add_subdirectory (lib/glad)
set (PHYANI_LIBS ${PHYANI_LIBS} glad)

# MESSAGE( STATUS ${PHYANI_LIBS} )
add_subdirectory (submodules)
# MESSAGE( STATUS ${PHYANI_LIBS} )

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_subdirectory (src)
