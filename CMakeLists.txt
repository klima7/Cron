cmake_minimum_required(VERSION 3.19)
project(cron)

set(CMAKE_CXX_STANDARD 14)
add_executable(cron siglog.cpp siglog.h task.cpp task.h cron.cpp cron.h interp.cpp)
target_link_libraries(cron pthread m)