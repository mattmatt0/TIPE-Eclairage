find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
find_package( OpenCV REQUIRED )
add_executable(training main.cpp)
target_link_libraries(training ${OpenCV_LIBS} )
