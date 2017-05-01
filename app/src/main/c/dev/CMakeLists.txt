# make sure this is put into where Gradle looks for stuff to put into .apk!lib/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O3 -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wundef -Wformat=2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-format-attribute -Wchar-subscripts -Wwrite-strings")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith -Wcast-align")

# make Gradle think this is a library
# (otherwise it won't include us into .apk!/lib)
add_library(pm3dev STATIC pm3dev.c pm3relay.c pm3uart.c)
add_executable(pm3relayd.so pm3relayd.c)
target_link_libraries(pm3relayd.so pm3dev)
