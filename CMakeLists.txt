cmake_minimum_required (VERSION 4.2)
project (Pomodoro)

SET(POMODORO_SOURCE_DIR /home/nur1znat/pomodoro)

FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)

INCLUDE_DIRECTORIES(
  ${Boost_INCLUDE_DIRS}
  ${POMODORO_SOURCE_DIR}/include
  )


set(CMAKE_CXX_FLAGS "-O3 -Wl,--allow-multiple-definition -std=c++20 -lpthread -Wall -pedantic -fpermissive -w ")

FILE(GLOB_RECURSE POMO_SRCS src/*.cpp include/*.h)

ADD_LIBRARY(POMO ${POMO_SRCS})

ADD_EXECUTABLE(${PROJECT_NAME} apps/pomoapp.cpp)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} POMO Boost::program_options)

