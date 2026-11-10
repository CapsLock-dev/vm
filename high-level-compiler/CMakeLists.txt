cmake_minimum_required(VERSION 3.25)

project(GigaCompiler)

set(EXEC_SRCS	
	src/main.cpp
)

add_subdirectory(modules/core)
add_subdirectory(modules/lexer)

add_executable(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_sources(${PROJECT_NAME} PRIVATE ${EXEC_SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    core 
)

option(BUILD_TESTING "Build unit and integration tests" OFF)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
 
