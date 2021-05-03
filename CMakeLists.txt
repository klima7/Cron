cmake_minimum_required(VERSION 3.19)
project(cron)

set(CMAKE_CXX_STANDARD 14)
add_executable(cron task.cpp task.h connect.h connect.cpp cron.cpp cron.h time.cpp time.h interp.cpp interp.h)
target_link_libraries(cron pthread m rt)
