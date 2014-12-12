#ifndef TEST_HEUR_H
#define TEST_HEUR_H

TEST(testHeurRelax);
TEST(testHeurGoalCount);
TEST(testHeurRelaxAdd);
TEST(testHeurRelaxMax);
TEST(testHeurRelaxFF);
TEST(testHeurRelaxLMCut);
TEST(protobufTearDown);

TEST_SUITE(TSHeur) {
    TEST_ADD(testHeurRelax),
    TEST_ADD(testHeurGoalCount),
    TEST_ADD(testHeurRelaxAdd),
    TEST_ADD(testHeurRelaxMax),
    TEST_ADD(testHeurRelaxFF),
    TEST_ADD(testHeurRelaxLMCut),
    TEST_ADD(protobufTearDown),
    TEST_SUITE_CLOSURE
};

#endif
