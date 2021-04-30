cmake_minimum_required(VERSION 3.19)
project(cron)

set(CMAKE_CXX_STANDARD 14)
add_executable(cron task.cpp task.h client.cpp client.h server.cpp server.h cron.h cron.cpp)
target_link_libraries(cron pthread m)
