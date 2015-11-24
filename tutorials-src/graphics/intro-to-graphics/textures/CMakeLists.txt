cmake_minimum_required(VERSION 3.1)

add_executable(tutorial
    "main.cpp"
)
if (MSVC)
    include(../../../../include_libs_with_freeimage.cmake)
endif (MSVC)

add_custom_command(TARGET tutorial POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
    	"${CMAKE_SOURCE_DIR}/texture.png"
        $<TARGET_FILE_DIR:tutorial>)   
add_custom_command(TARGET tutorial POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
    	"${CMAKE_SOURCE_DIR}/texture.png"
        ${CMAKE_BINARY_DIR})   
# vim: ts=4 sw=4 et
