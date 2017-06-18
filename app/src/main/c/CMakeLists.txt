# make sure pm3relayd is put into where Gradle looks for stuff to put into .apk!lib/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O3 -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wundef -Wformat=2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-format-attribute -Wchar-subscripts -Wwrite-strings")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith -Wcast-align")

add_executable(libpm3relayd.so pm3relayd.c pm3uart.c pm3util.c)

add_library(pm3jni SHARED glue.c pm3jni.c pm3dev.c pm3uart.c pm3util.c)
target_link_libraries(pm3jni proxmark3 android atomic log m)
