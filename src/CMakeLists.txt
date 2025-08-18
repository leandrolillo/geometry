add_library(${LIBRARY_NAME} INTERFACE)
add_subdirectory(geometry)
add_subdirectory(collisionDetection)

FetchContent_Declare(
    math
    GIT_REPOSITORY https://github.com/leandrolillo/math.git
    GIT_TAG master
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(math)


# library link dependencies
target_link_libraries(geometry INTERFACE math)


