#include "strings.h"
#include "mm/simple_malloc.h"
#include "utils.h"
#include "bitops.h"

#define DEBUG

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

//deprecated
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

int test_cmp(const void* a, const void* b){
    return (*(int*)a - *(int*)b);
}

struct a{
    int x;
    int y;
};
int test_cmp2(const void* a, const void* b){
    return (*(struct a*) a).x - (*(struct a*)b).x;
}
void test_quick_sort(){
    int arr[] = {5, 4, 3, 2, 1};
    quickSort(arr, 0, 4, sizeof(int), test_cmp);
    ASSERT_EQ(1, arr[0], "Should be 1\n");
    ASSERT_EQ(2, arr[1], "Should be 2\n");
    ASSERT_EQ(3, arr[2], "Should be 3\n");
    ASSERT_EQ(4, arr[3], "Should be 4\n");
    ASSERT_EQ(5, arr[4], "Should be 5\n");
    struct a arr2[3] = {
        {1, 2},
        {3, 4},
        {2, 3}
    };
    quickSort(arr2, 0, 2 , sizeof(struct a), test_cmp2);
    ASSERT_EQ(1, arr2[0].x, "Should be 1\n");
    ASSERT_EQ(2, arr2[1].x, "Should be 2\n");
    ASSERT_EQ(3, arr2[2].x, "Should be 3\n");
}


void test_bit_ops(){
    d_printfln("Testing bit ops");
    d_printfln("www %d", (1 << ((1)%8)));
    uint8_t a[2] = {0};
    for(int i = 0; i < 16; i++){
        d_printfln("before set %d", a[0]);
        ASSERT(CHECK_BIT(a, i) == 0, "%d Should be 0\n", i);
        SET_BIT(a, i);
        d_printfln("after set %d %d", a[0], CHECK_BIT(a, i));
        
        ASSERT(CHECK_BIT(a, i), "%d Should be 1\n", i);
    }

    for(int i=0; i < 16; i++){
        CLEAR_BIT(a, i);
        ASSERT(CHECK_BIT(a, i) == 0, "%d Should be 0\n", i);
    }

    uint8_t b[8] = {0};
    SET_BIT(b, 32);
    ASSERT(CHECK_BIT(b, 32), "Should be 1\n");
    ASSERT(!CHECK_BIT(b, 33), "Should be 0\n");
    ASSERT(!CHECK_BIT(b, 34), "Should be 0\n");
    SET_BIT(b, 33);
    ASSERT(CHECK_BIT(b, 33), "Should be 1\n");
    ASSERT(!CHECK_BIT(b, 34), "Should be 0\n");
    d_printfln("Testing bit ops done");
}

void run_tests(){
    test_bit_ops();
    test_strtok();
    test_quick_sort();
}



#else
void run_tests(){}
#endif
