add_library(search 
	"rmp-all.cpp" 
	"space.cpp"
	"segments.cpp"
	"heuristic.cpp" 
	"agent.cpp")

set_property(TARGET search PROPERTY CXX_STANDARD 17)
target_include_directories(search PRIVATE ${RMP_include_dirs})

add_executable(mapf_vis mapf_vis.cpp "hog2-utils/ScenarioLoader.cpp")
target_compile_definitions(mapf_vis PRIVATE TEST_DATA_PATH="${CMAKE_SOURCE_DIR}/test/test-data")

set_property(TARGET mapf_vis PROPERTY CXX_STANDARD 17)
target_include_directories(mapf_vis PRIVATE ${RMP_include_dirs})
target_link_libraries(mapf_vis PRIVATE search)

add_subdirectory("prototyping")