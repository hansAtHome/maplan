#ifndef TEST_MA_COMM_QUEUE
#define TEST_MA_COMM_QUEUE

TEST(maCommQueue1);
TEST(protobufTearDown);

TEST_SUITE(TSMACommQueue){
    TEST_ADD(maCommQueue1),
    TEST_ADD(protobufTearDown),
    TEST_SUITE_CLOSURE
};

#endif
