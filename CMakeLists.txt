cmake_minimum_required(VERSION 3.5)
project(RoundtripDemo C)

set(CMAKE_C_STANDARD 99)

set(CYCLONEDDS_DIR /opt/cyclonedds)

include_directories(${CYCLONEDDS_DIR}/include)
link_directories(${CYCLONEDDS_DIR}/lib)

set(SOURCES
    publisher.c
    subscriber.c
    NodeStatusReport.c
)

add_executable(publisher publisher.c NodeStatusReport.c)
add_executable(subscriber subscriber.c NodeStatusReport.c)

target_link_libraries(publisher ddsc)
target_link_libraries(subscriber ddsc)

