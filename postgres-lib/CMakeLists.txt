﻿cmake_minimum_required (VERSION 3.8)

# Hack to locate boost libs. Find_boost will not locate libs given filenames are a complete mess.
# Has been like this for a while...
# This is site specific and probably will not work anywhere else (of course).
set(BOOST_OPTIONS thread system date_time regex chrono)
set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LISB ON)
set(Boost_NO_SYSTEM_PATHS ON)
set(BOOST_LIB_PREFIX "libboost_")
if(WIN32)		
	set(BOOST_ROOT "D:/boost_1_66_0")
	set(BOOST_LIB_SUFFIX "-vc141-mt-x64-1_66.lib")
else()
	set(BOOST_ROOT "/groups/RCC/uqocairn/boost_1_66_0")
	set(BOOST_LIB_SUFFIX "-gcc71-mt-x64-1_66.a")	
endif()
find_package(Boost)
foreach(lib ${BOOST_OPTIONS})
	message(STATUS ${Boost_LIBRARY_DIRS}/${BOOST_LIB_PREFIX}${lib}${BOOST_LIB_SUFFIX})
	list(APPEND BOOST_LIBS ${Boost_LIBRARY_DIRS}/${BOOST_LIB_PREFIX}${lib}${BOOST_LIB_SUFFIX})
endforeach(lib)

find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(PostgreSQL MODULE REQUIRED)

add_library (postgreslib "postgres-lib.cpp" "postgres-lib.h" "MeshMakerDB.cpp" "MeshMakerDB.h")
target_include_directories (postgreslib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(postgreslib ${PostgreSQL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(postgreslib ${PostgreSQL_LIBRARIES} ${BOOST_LIBS})

target_link_libraries(postgreslib fmt::fmt nlohmann_json)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY CACHE_VARIABLES)
foreach(dir ${dirs})
  message(STATUS "property '${dir} : ${${dir}}'")
endforeach()

