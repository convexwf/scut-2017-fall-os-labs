cmake_minimum_required(VERSION 2.8.4)
project(exp_2)
set(target_1 producer_consumer)
set(target_2 barber)
set(target_3 reader_writer)
include_directories(include)
find_package(Threads REQUIRED)  
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
add_executable(${target_1} src/produce_consume.cpp src/sem_p.cpp)
add_executable(${target_2} src/barber_sem.cpp)
add_executable(${target_3} src/reader_writer.cpp src/sem_p.cpp)

if(THREADS_HAVE_PTHREAD_ARG)  
  set_property(TARGET ${target_2} PROPERTY COMPILE_OPTIONS "-pthread")  
  set_property(TARGET ${target_2} PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")  
endif()  
if(CMAKE_THREAD_LIBS_INIT)  
  target_link_libraries(${target_2} "${CMAKE_THREAD_LIBS_INIT}")  
endif()  

