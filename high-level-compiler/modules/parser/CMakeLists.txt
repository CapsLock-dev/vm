add_library(parser STATIC)

target_sources(parser
	PRIVATE
        src/parser.cpp
        src/ast.cpp
        src/arena_allocator.cpp
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS include
		FILES
			include/parser/parser.hpp
			include/parser/ast.hpp
			include/parser/arena_allocator.hpp
)

target_link_libraries(parser PRIVATE 
   core lexer
)

target_compile_features(parser PUBLIC cxx_std_23)
