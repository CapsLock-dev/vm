add_library(core STATIC)

target_sources(core
	PRIVATE
    	src/logger.cpp
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS include
		FILES
			include/core/logger.hpp
)

target_compile_features(core PUBLIC cxx_std_23)
