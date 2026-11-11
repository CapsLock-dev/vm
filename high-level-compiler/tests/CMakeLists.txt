find_package(Catch2 CONFIG REQUIRED)

include(CTest)
include(Catch)

add_subdirectory(unit/point)
add_subdirectory(unit/figure)
