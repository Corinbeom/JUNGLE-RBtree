# test/CMakeLists.txt
add_executable(test-rbtree test-rbtree.c)
target_include_directories(test-rbtree PRIVATE ../src)
target_link_libraries(test-rbtree rbtree)
add_executable(test-rbtree
        test-rbtree.c
)

target_include_directories(test-rbtree PRIVATE ../src)
target_link_libraries(test-rbtree rbtree)
