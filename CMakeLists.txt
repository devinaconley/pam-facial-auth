cmake_minimum_required(VERSION 3.5)
project(pam-facial-auth)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(run_training run_training.cpp)
target_link_libraries(run_training ${OpenCV_LIBS})

add_executable(run_test run_test.cpp)
target_link_libraries(run_test -lpam -lpam_misc ${OpenCV_LIBS})

add_library(facialauth SHARED src/FacialAuth.cpp)
target_link_libraries(facialauth ${OpenCV_LIBS})
set_target_properties(facialauth PROPERTIES PREFIX "")