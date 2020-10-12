find_package(OpenCV)

ADD_EXECUTABLE(test.out main.cpp)
SET(CMAKE_CXX_FLAGS "-pthread -I/usr/local/include -L/usr/local/lib -lwiringPi")
include_directories(/usr/local/include/)

target_link_libraries(test.out ${OpenCV_LIBS} /usr/local/lib/libwiringPi.so /usr/local/lib/libwiringPiDev.so)
