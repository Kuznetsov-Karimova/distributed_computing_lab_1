cmake_minimum_required(VERSION 3.28)
project(peplya LANGUAGES C)

set(SCR_DIR ./src)
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_FLAGS "-lm")

include_directories(include)

add_executable(task_1 ${SCR_DIR}/pi.c)
add_executable(task_2 ${SCR_DIR}/mandel.c)
add_executable(task_3 ${SCR_DIR}/rwlock.c /
     ${SCR_DIR}/my_pth_ll_rwl.c ${SCR_DIR}/my_rand.c)
add_executable(task_3_pth ${SCR_DIR}/rwlock.c /
     ${SCR_DIR}/pth_ll_rwl.c ${SCR_DIR}/my_rand.c)