ADD_EXECUTABLE(VolumeDataTest 
		VolumeDataTest.cpp 
		VolumeInfoTest.cpp
		volume_data_test_main.cpp)
INCLUDE_DIRECTORIES( ../../include)
ADD_TEST(VolumeDataTest VolumeDataTest) #for CTest
	





