cmake_minimum_required(VERSION 3.4)

project(network)

file(GLOB all_src "*.hpp" "*.cpp")

set(SERVER_SOURCES server.cpp)
set(CLIENT_SOURCES client.cpp)
add_executable(t-server ${SERVER_SOURCES})
add_executable(t ${CLIENT_SOURCES})

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

find_package(SFML 2 REQUIRED graphics window network system)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(t-server ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
    target_link_libraries(t ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()

target_link_libraries(t-server pthread)
target_link_libraries(t pthread)

install(TARGETS t-server t DESTINATION bin)