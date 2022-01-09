add_executable(segment_testing 
	"segment_testing.cpp")

set_property(TARGET segment_testing PROPERTY CXX_STANDARD 17)
target_include_directories(segment_testing PRIVATE ${RMP_include_dirs})
target_link_libraries(segment_testing PRIVATE search)
