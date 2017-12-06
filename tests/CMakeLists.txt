INCLUDE_DIRECTORIES( ../include)
file(GLOB TEST_SOURCES "*.cpp")
ADD_EXECUTABLE(mi4test ${TEST_SOURCES} mi4test_main.cpp)
ADD_TEST(mi4test mi4test)
