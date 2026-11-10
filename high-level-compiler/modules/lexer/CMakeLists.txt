add_library(lexer STATIC)

target_sources(lexer
	PRIVATE
    	src/lexer.cpp
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS include
		FILES
			include/lexer/lexer.hpp
)

target_compile_features(lexer PUBLIC cxx_std_23)
