#ifndef TEST_HEUR_ADMISSIBLE_H
#define TEST_HEUR_ADMISSIBLE_H

TEST(testHeurAdmissibleLMCut);
TEST(testHeurAdmissibleMax);
TEST(protobufTearDown);

TEST_SUITE(TSHeurAdmissible){
    TEST_ADD(testHeurAdmissibleLMCut),
    TEST_ADD(testHeurAdmissibleMax),
    TEST_ADD(protobufTearDown),
    TEST_SUITE_CLOSURE
};

#endif /* TEST_HEUR_ADMISSIBLE_H */
