include_directories (${PHYANI_INCLUDES})

file(GLOB PHYANI_SOURCES "*.cpp")
file(GLOB PHYANI_HEADERS "*.h")

add_definitions (-DGLFW_INCLUDE_NONE)
add_executable (phyani_playground ${PHYANI_SOURCES} ${PHYANI_HEADERS})
target_link_libraries (phyani_playground  ${PHYANI_LIBS})

add_custom_command(TARGET phyani_playground POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders"
)
