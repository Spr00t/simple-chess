cmake_minimum_required(VERSION 2.8.12)

project(simple-chess)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set (CMAKE_BUILD_TYPE Debug)
# Boost
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost COMPONENTS system)
if(Boost_FOUND)
    if (Boost_FIND_COMPONENTS)
          message("Found Boost components:")
          message("   ${Boost_FIND_COMPONENTS}")
        endif()
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set (CMAKE_CXX_STANDARD 14)

message("BOOST is here: ${Boost_INCLUDE_DIRS}")

message("BOOST is here: ${Boost_LIBRARIES}")


FILE(GLOB_RECURSE HEADER "*engine/*.h")
FILE(GLOB_RECURSE SRC "engine/*.cpp")
include_directories("engine")

add_executable(${PROJECT_NAME} ${SRC} ${HEADER})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
