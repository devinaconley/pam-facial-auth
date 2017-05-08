cmake_minimum_required(VERSION 2.8)
project(pam-facial-auth)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(run_training run_training.cpp src/FaceRecWrapper.cpp)
target_link_libraries(run_training ${OpenCV_LIBS})

add_executable(run_test run_test.cpp)
target_link_libraries(run_test -lpam -lpam_misc ${OpenCV_LIBS})

add_library(facialauth SHARED src/FacialAuth.cpp src/FaceRecWrapper.cpp)
target_link_libraries(facialauth ${OpenCV_LIBS})
set_target_properties(facialauth PROPERTIES PREFIX "")