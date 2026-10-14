cmake_minimum_required(VERSION 4.4)
project(opengles_experiments VERSION 0.1.0)

include(CTest)
enable_testing()

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

include_directories(${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc)


set(SOURCES 
    src/main.c)

add_executable(opengles_experiments ${SOURCES})
target_link_libraries(opengles_experiments)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)