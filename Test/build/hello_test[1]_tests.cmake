add_test( HelloTest.BasicAssertions [==[/Users/michaelchai/Desktop/UNI Second Year/Term 3/DESN2000/build/hello_test]==] [==[--gtest_filter=HelloTest.BasicAssertions]==] --gtest_also_run_disabled_tests)
set_tests_properties( HelloTest.BasicAssertions PROPERTIES WORKING_DIRECTORY [==[/Users/michaelchai/Desktop/UNI Second Year/Term 3/DESN2000/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( hello_test_TESTS HelloTest.BasicAssertions)
