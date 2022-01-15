add_library(search 
	"rmp-all.cpp" 
	"space.cpp"
	"segments.cpp"
	"heuristic.cpp" 
	"agent.cpp")

set_property(TARGET search PROPERTY CXX_STANDARD 17)
target_include_directories(search PRIVATE ${RMP_include_dirs})

add_subdirectory("prototyping")