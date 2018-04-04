cmake_minimum_required (VERSION 2.8)

#Déclaration du projet
project (MqttOwfs)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(Mosquitto REQUIRED)
find_package(Mosquittopp REQUIRED)
 
add_subdirectory(src)
add_subdirectory(test)
