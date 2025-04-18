# src/CMakeLists.txt
add_library(rbtree STATIC rbtree.c)

add_executable(driver driver.c)
target_link_libraries(driver rbtree)
add_library(rbtree
        rbtree.c
        rbtree.h
)
