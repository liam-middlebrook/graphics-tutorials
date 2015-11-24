file(
    COPY ../../../../glew-1.13.0-win32.zip
    DESTINATION ${CMAKE_BINARY_DIR}
)
execute_process(
    COMMAND unzip ${CMAKE_BINARY_DIR}/glew-1.13.0-win32.zip
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
file(
    COPY ../../../../glfw-3.1.2.bin.WIN32.zip
    DESTINATION ${CMAKE_BINARY_DIR}
)
execute_process(
    COMMAND unzip ${CMAKE_BINARY_DIR}/glfw-3.1.2.bin.WIN32.zip
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
file(
    COPY ../../../../glm-0.9.7.1.zip
    DESTINATION ${CMAKE_BINARY_DIR}
)
execute_process(
    COMMAND unzip ${CMAKE_BINARY_DIR}/glm-0.9.7.1.zip
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
file(
    COPY ../../../../FreeImage3170Win32Win64.zip
    DESTINATION ${CMAKE_BINARY_DIR}
)
execute_process(
    COMMAND unzip ${CMAKE_BINARY_DIR}/FreeImage3170Win32Win64.zip
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_libraries(tutorial
    ${CMAKE_BINARY_DIR}/glew-1.13.0/lib/Release/Win32/glew32.lib
    ${CMAKE_BINARY_DIR}/glfw-3.1.2.bin.WIN32/lib-vc2015/glfw3.lib
    ${CMAKE_BINARY_DIR}/FreeImage/Dist/x32/FreeImage.lib
    opengl32.lib
)

include_directories(
    ${CMAKE_BINARY_DIR}/glew-1.13.0/include
    ${CMAKE_BINARY_DIR}/glfw-3.1.2.bin.WIN32/include
    ${CMAKE_BINARY_DIR}/glm
    ${CMAKE_BINARY_DIR}/FreeImage/Dist/x32
)

add_custom_command(TARGET tutorial POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${CMAKE_BINARY_DIR}/glew-1.13.0/bin/Release/Win32/glew32.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:tutorial>)   

add_custom_command(TARGET tutorial POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${CMAKE_SOURCE_DIR}/VertexShader.glsl"      # <--this is in-file
        $<TARGET_FILE_DIR:tutorial>)   

add_custom_command(TARGET tutorial POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${CMAKE_SOURCE_DIR}/FragmentShader.glsl"      # <--this is in-file
        $<TARGET_FILE_DIR:tutorial>)   

add_custom_command(TARGET tutorial POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${CMAKE_SOURCE_DIR}/VertexShader.glsl"      # <--this is in-file
        ${CMAKE_BINARY_DIR})   

add_custom_command(TARGET tutorial POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${CMAKE_SOURCE_DIR}/FragmentShader.glsl"      # <--this is in-file
        ${CMAKE_BINARY_DIR})   
        
add_custom_command(TARGET tutorial POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
    	"${CMAKE_BINARY_DIR}/FreeImage/Dist/x32/FreeImage.dll"
        $<TARGET_FILE_DIR:tutorial>)   
