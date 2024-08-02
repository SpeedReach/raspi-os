#include "strings.h"
#include "mm/simple_malloc.h"

#ifdef DEBUG
void test_strtok(){
    char str[] = " This is a test";
    char* token = strtok(str, " ");
    ASSERT(strcmp(token, "") == 0, "Should be Empty\n");
    token = strtok(NULL, " ");
    ASSERT(strcmp(token, "This") == 0, "Should be This\n");
    token = strtok(NULL, " ");
    ASSERT(strcmp(token, "is") == 0, "Should be is\n");
    token = strtok(NULL, " ");
    ASSERT(strcmp(token, "a") == 0, "Should be a\n");
    token = strtok(NULL, " ");
    ASSERT(strcmp(token, "test") == 0, "Should be test\n");
    token = strtok(NULL, " ");
    ASSERT(token == NULL, "Should be NULL\n");
}

void test_simple_malloc(){
    int* a = simple_malloc(sizeof(int));
    *a = 5;
    ASSERT_EQ(5, *a, "Should be 5\n");
    int* b = simple_malloc(sizeof(int));
    *b = 6;
    ASSERT_EQ(6, *b, "Should be 6\n");
    ASSERT_EQ(5, *a, "Should be 5\n");
    ASSERT_EQ(6, *b, "Should be 6\n");
    int* c = simple_malloc(sizeof(int));
    *c = 7;
    ASSERT_EQ(7, *c, "Should be 7\n");
    ASSERT_EQ(5, *a, "Should be 5\n");
    ASSERT_EQ(6, *b, "Should be 6\n");
}


void run_tests(){
    test_simple_malloc();
    test_strtok();
}
#elif
void run_tests(){}
#endif
