if(EXISTS "/Users/michaelchai/Documents/PlatformIO/Projects/221101-231314-wiscore_rak4631/build/main_test[1]_tests.cmake")
  include("/Users/michaelchai/Documents/PlatformIO/Projects/221101-231314-wiscore_rak4631/build/main_test[1]_tests.cmake")
else()
  add_test(main_test_NOT_BUILT main_test_NOT_BUILT)
endif()
