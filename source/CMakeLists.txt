add_library(search 
	"rmp-all.cpp" 
	"space.cpp"
)

target_include_directories(search PRIVATE ${RMP_include_dirs})