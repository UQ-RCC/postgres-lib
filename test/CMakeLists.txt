cmake_minimum_required (VERSION 3.8)

add_executable(dbtest "test.cpp")
target_link_libraries (dbtest LINK_PUBLIC postgreslib)