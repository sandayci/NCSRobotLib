cmake_minimum_required(VERSION 2.8)

project (move_little_pushbot)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -DDEVICE_MN256R1")

#add Boost
find_package(Boost REQUIRED COMPONENTS program_options filesystem)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#add RobotLib
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/NCSRobotLib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/NCSRobotLib/paex-dist-cc/lib/aerctl
    ${CMAKE_CURRENT_SOURCE_DIR}/NCSRobotLib/paex-dist-cc/server/lib/cjson)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/NCSRobotLib)

# add project files
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/NCSRobotLib/paex-dist-cc/server/lib/cjson/*)
add_library(${PROJECT_NAME}_lib SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME}_lib Robot-lib ${Boost_LIBRARIES})

# build executables
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
		)
target_link_libraries(${PROJECT_NAME}
                      ${PROJECT_NAME}_lib
                      Robot-lib 
                      ${Boost_LIBRARIES}
			)

# TEST VISUALIZATION
#add OpenCV
find_package( OpenCV REQUIRED )
add_executable(draw_architecture 
               ${CMAKE_CURRENT_SOURCE_DIR}/draw_architecture.cpp)
target_link_libraries(draw_architecture
		      Robot-lib
                      ${PROJECT_NAME}_lib
                      ${OpenCV_LIBS}
                      ${Boost_LIBRARIES}
                      )

