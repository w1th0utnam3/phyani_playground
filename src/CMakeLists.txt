# Add all source and header files recursively
file(GLOB_RECURSE PHYANI_SOURCES "*.cpp")
file(GLOB_RECURSE PHYANI_HEADERS "*.h")

# Prevent clash of GLFW and GLAD includes
add_definitions (-DGLFW_INCLUDE_NONE)

# Create the phyani target
add_executable (phyani_playground ${PHYANI_SOURCES} ${PHYANI_HEADERS})
target_link_libraries (phyani_playground  ${PHYANI_LIBS})

# Add project include directories
target_include_directories (phyani_playground PUBLIC ${PHYANI_INCLUDES})
# Add subdirectories as include directories
target_include_directories (phyani_playground PUBLIC "render_backend")
target_include_directories (phyani_playground PUBLIC "scenes")

# Copy shaders to build directory
add_custom_command(TARGET phyani_playground POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders"
)
