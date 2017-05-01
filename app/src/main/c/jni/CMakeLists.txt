set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O3 -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wundef -Wformat=2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-format-attribute -Wchar-subscripts -Wwrite-strings")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith -Wcast-align")

add_library(pm3jni SHARED pm3jni.c pm3jniworker.c glue.c)
target_link_libraries(pm3jni proxmark3 pm3dev android atomic log m)
