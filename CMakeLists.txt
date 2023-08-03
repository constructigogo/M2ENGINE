cmake_minimum_required(VERSION 3.25)
project(Engine)

set(CMAKE_CXX_STANDARD 20)



#set(BUILD_SHARED_LIBS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_OBJ_IMPORTER TRUE CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER TRUE CACHE BOOL "" FORCE)



set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

set(SHADER_PLATFORM "")
if (WIN32)
    set(SHADER_PLATFORM "WINDOWS")
elseif (UNIX)
    set(SHADER_PLATFORM "LINUX")
endif()

# OpenGL
find_package(OpenGL REQUIRED)

include_directories(OpenGL)
#include_directories(OpenGL/include) # OpenGL/include has to contain the required OpenGL's .h files
#include_directories(OpenGL/lib) # OpenGL/lib has to contain the required OpenGL's .lib files

add_subdirectory(libraries/)


file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/Engine/*.cpp ${PROJECT_SOURCE_DIR}/Engine/*.h)
file(GLOB_RECURSE INC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/includes/*.h ${PROJECT_SOURCE_DIR}/src/*.cpp)

bgfx_compile_shader_to_header(
        TYPE VERTEX
        SHADERS shaders/v_simple.vert
        VARYING_DEF ${CMAKE_SOURCE_DIR}/shaders/varying.def.sc
        OUTPUT_DIR ${CMAKE_SOURCE_DIR}/shaders/generated
)

bgfx_compile_shader_to_header(
        TYPE FRAGMENT
        SHADERS shaders/f_simple.frag
        VARYING_DEF ${CMAKE_SOURCE_DIR}/shaders/varying.def.sc
        OUTPUT_DIR ${CMAKE_SOURCE_DIR}/shaders/generated
)

file(GLOB_RECURSE SHADERS_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/shaders/*.frag ${PROJECT_SOURCE_DIR}/shaders/*.vert)
set(RESULTS "")
foreach(SHADER_FILE ${SHADERS_FILES})

    get_filename_component(SHADER_FILE_BASENAME ${SHADER_FILE} NAME)
    get_filename_component(SHADER_FILE_NAME_WE ${SHADER_FILE} NAME_WE)
    get_filename_component(SHADER_FILE_ABSOLUTE ${SHADER_FILE} ABSOLUTE)
    get_filename_component(SHADER_FILE_EXT ${SHADER_FILE} EXT)
    set(EXT_TYPE "")
    if (${SHADER_FILE_EXT} STREQUAL ".vert")
        set(EXT_TYPE "VERTEX")
    elseif (${SHADER_FILE_EXT} STREQUAL ".frag")
        set(EXT_TYPE "FRAGMENT")
    endif ()

    list(APPEND RESULTS ${PROJECT_SOURCE_DIR}/shaders/generated/${SHADER_FILE_BASENAME}.bin)
    _bgfx_shaderc_parse(
        RESULT
        FILE ${SHADER_FILE_ABSOLUTE}
        OUTPUT ${PROJECT_SOURCE_DIR}/shaders/generated/${SHADER_FILE_BASENAME}.bin
        ${EXT_TYPE}
        ${SHADER_PLATFORM}
        VARYING_DEF ${PROJECT_SOURCE_DIR}/shaders/varying.def.sc
    )
    add_custom_command(
            OUTPUT ${PROJECT_SOURCE_DIR}/shaders/generated/${SHADER_FILE_BASENAME}.bin
            COMMAND shaderc ${RESULT}
            DEPENDS ${PROJECT_SOURCE_DIR}/shaders/varying.def.sc
    )
endforeach ()

if(PROJECT_IS_TOP_LEVEL OR TRUE)
    add_executable(Engine main.cpp ${SRC_FILES} ${INC_FILES} ${RESULTS})
    set_property(TARGET Engine PROPERTY ENABLE_EXPORTS 1)
else ()
    #add_library(Engine main.cpp ${SRC_FILES} ${INC_FILES} ${RESULTS})
endif()


#target_include_dir(Engine)
target_link_libraries(Engine PUBLIC assimp)
target_link_libraries(Engine PUBLIC bgfx)
target_link_libraries(Engine PUBLIC bx)
target_link_libraries(Engine PUBLIC bimg)
target_link_libraries(Engine PUBLIC EnTT::EnTT)
target_link_libraries(Engine PUBLIC glad)
target_link_libraries(Engine PUBLIC glfw)
target_link_libraries(Engine PUBLIC glm::glm)
target_link_libraries(Engine PUBLIC spdlog)
target_link_libraries(Engine PUBLIC example-common)

target_link_libraries(Engine PUBLIC OpenGL::GL)



