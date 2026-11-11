add_executable(unit_figure)

target_sources(unit_figure
	PRIVATE
        figure_test.cpp
)

target_link_libraries(unit_figure
    PRIVATE
        geometry
        Catch2::Catch2WithMain
)

catch_discover_tests(unit_figure)
