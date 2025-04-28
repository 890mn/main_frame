cmake_minimum_required(VERSION 3.5)
project(RoundtripDemo C)

set(CMAKE_C_STANDARD 99)

set(CYCLONEDDS_DIR /opt/cyclonedds)

if(EXISTS "${CYCLONEDDS_DIR}/lib64")
    set(CYCLONEDDS_LIB_DIR "${CYCLONEDDS_DIR}/lib64")
elseif(EXISTS "${CYCLONEDDS_DIR}/lib")
    set(CYCLONEDDS_LIB_DIR "${CYCLONEDDS_DIR}/lib")
else()
    message(FATAL_ERROR "CycloneDDS libraries not found under ${CYCLONEDDS_DIR}/lib or lib64")
endif()

include_directories(${CYCLONEDDS_DIR}/include)
link_directories(${CYCLONEDDS_LIB_DIR})

set(SOURCES
    publisher.c
    subscriber.c
    NodeStatusReport.c
)

add_executable(publisher publisher.c NodeStatusReport.c)
add_executable(subscriber subscriber.c NodeStatusReport.c)

target_link_libraries(publisher ddsc pthread)
target_link_libraries(subscriber ddsc pthread)
