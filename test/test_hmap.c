#include "unity.h"
#include "hmap.h"
#include "utils.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void hashMapTestBasic() {
    HashMap_t* map = createHashMap();
    cleanupHashMap(&map);    
}

void hashMapTestInsert() {
    HashMap_t* map = createHashMap();

    hashMapInsert(map, "hello", cloneString("my value"));
    hashMapInsert(map, "test1", cloneString("my value 1"));
    hashMapInsert(map, "test1", cloneString("my value 4"));
    hashMapInsert(map, "test2", cloneString("my value 2"));   
    hashMapInsert(map, "test3", cloneString("my value 3"));

    TEST_ASSERT_EQUAL_INT_MESSAGE(4, map->itemCnt, "Wrong item count");  
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, map->numBuckets, "Wrong number of buckets");  
    TEST_ASSERT_EQUAL_STRING("my value", hashMapGet(map, "hello")); 
    TEST_ASSERT_EQUAL_STRING("my value 4", hashMapGet(map, "test1")); 
    TEST_ASSERT_EQUAL_STRING("my value 2", hashMapGet(map, "test2")); 
    TEST_ASSERT_EQUAL_STRING("my value 3", hashMapGet(map, "test3")); 
    cleanupHashMap(&map);
}

// not needed when using generate_test_runner.rb
int main(void) {
   UNITY_BEGIN();
   RUN_TEST(hashMapTestBasic);
   RUN_TEST(hashMapTestInsert);
   return UNITY_END();
}