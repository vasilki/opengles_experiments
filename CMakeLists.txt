cmake_minimum_required(VERSION 4.4)
project(opengles_experiments VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

#add_subdirectory(eswb/src/lib)

if (MSVC)
    # warning level 4 and all warnings as errors
    #add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    #add_compile_options(-Wall -Wextra -pedantic -Werror)
    #add_compile_options(-Wall)
endif()

add_compile_options(-D_GNU_SOURCE)

find_package(SDL2 CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc)


set(SOURCES 
    src/main.c
    src/init.c
    src/gles2_loader.c
    src/shader.c
    src/draw.c
    src/gglib.c
    src/matrix.c    
)

add_executable(opengles_experiments ${SOURCES})
target_link_libraries(opengles_experiments PRIVATE SDL2::SDL2)

add_custom_command(TARGET opengles_experiments POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:opengles_experiments>/assets"
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)