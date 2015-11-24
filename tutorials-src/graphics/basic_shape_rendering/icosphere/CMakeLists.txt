cmake_minimum_required(VERSION 3.1)

add_executable(tutorial
    "main.cpp"
    "GameObject.cpp"
    "Model.cpp"
)
if (MSVC)
    include(../../../../include_libs_without_freeimage.cmake)
endif (MSVC)

# vim: ts=4 sw=4 et
