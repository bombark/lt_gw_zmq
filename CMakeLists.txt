cmake_minimum_required(VERSION 3.0)
project(lt_zmq)


add_library(lt_zmq SHARED
    ./src/lt_zmq_common.c 
    ./src/lt_zmq_topic.c 
    ./src/lt_zmq_socket.c 
    )

target_compile_options(lt_zmq PUBLIC -O1)
target_link_libraries(lt_zmq zmq lt_api)

install(TARGETS lt_zmq DESTINATION lib)

# test
# enable_testing()
# add_subdirectory(./tests)