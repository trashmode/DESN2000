add_test( FunctionTest.BasicAssertion [==[/Users/michaelchai/Desktop/UNI Second Year/Term 3/DESN2000/DESN2000/src/build/main_test]==] [==[--gtest_filter=FunctionTest.BasicAssertion]==] --gtest_also_run_disabled_tests)
set_tests_properties( FunctionTest.BasicAssertion PROPERTIES WORKING_DIRECTORY [==[/Users/michaelchai/Desktop/UNI Second Year/Term 3/DESN2000/DESN2000/src/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( HelloTest.BasicAssertions [==[/Users/michaelchai/Desktop/UNI Second Year/Term 3/DESN2000/DESN2000/src/build/main_test]==] [==[--gtest_filter=HelloTest.BasicAssertions]==] --gtest_also_run_disabled_tests)
set_tests_properties( HelloTest.BasicAssertions PROPERTIES WORKING_DIRECTORY [==[/Users/michaelchai/Desktop/UNI Second Year/Term 3/DESN2000/DESN2000/src/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( main_test_TESTS FunctionTest.BasicAssertion HelloTest.BasicAssertions)
