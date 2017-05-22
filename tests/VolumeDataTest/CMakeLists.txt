ADD_EXECUTABLE(VolumeDataTest
                WatershedTest.cpp
		VolumeDataTest.cpp
		VolumeInfoTest.cpp
		volume_data_test_main.cpp)
INCLUDE_DIRECTORIES( ../../include)
ADD_TEST(VolumeDataTest VolumeDataTest) #for CTest
