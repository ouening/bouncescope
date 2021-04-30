cmake_minimum_required(VERSION 3.15)

project(ball)

add_executable(ball WIN32 collision ballssim.cpp bouncescope.cpp bsrc.rc)
# 使用timeGetTime函数需要链接WinMMLib库
target_link_libraries(ball "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.18362.0/um/x86/WinMM.Lib")