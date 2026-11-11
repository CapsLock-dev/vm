add_library(lexer STATIC)

target_sources(lexer
	PRIVATE
    	src/lexer.cpp
        src/tokenizer.cpp
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS include
		FILES
			include/lexer/lexer.hpp
			include/lexer/tokens.hpp
			include/lexer/tokenizer.hpp
)

target_compile_features(lexer PUBLIC cxx_std_23)
