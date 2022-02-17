cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 11)
 
project(ffprobe VERSION 1.0.0)
include_directories(${CMAKE_JS_INC}) 

find_package(PkgConfig REQUIRED) 
pkg_check_modules(FFMPEG REQUIRED libavcodec libavutil libavformat)


execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

file(GLOB SOURCE_FILES "src/*.cc") 

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} PRIVATE ${FFMPEG_LIBRARIES})
target_link_directories(${PROJECT_NAME} BEFORE PUBLIC ${FFMPEG_LIBRARY_DIRS})

target_include_directories(${PROJECT_NAME} PUBLIC ${FFMPEG_INCLUDE_DIRS} PRIVATE ${NODE_ADDON_API_DIR})
target_compile_options(${PROJECT_NAME} PUBLIC ${FFMPEG_CFLAGS})


add_definitions(-DNAPI_VERSION=3)

