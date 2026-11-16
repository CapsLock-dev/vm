add_executable(lexer_test)

target_sources(lexer_test
	PRIVATE
        lexer_test.cpp
)

target_link_libraries(lexer_test
    PRIVATE
        lexer Catch2::Catch2WithMain
)

catch_discover_tests(lexer_test)
