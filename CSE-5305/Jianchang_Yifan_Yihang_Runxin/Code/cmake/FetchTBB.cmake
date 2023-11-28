include(FetchContent)
set(FETCHCONTENT_QUIET ON)

message(STATUS "Cloning External Project: oneTBB")

FetchContent_Declare(
        tbb
        GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
        GIT_TAG v2021.11.0
)

option(TBB_TEST "Build and run TBB tests" OFF)
if (TBB_TEST)
    set(TBB_TESTS ON)
endif ()

FetchContent_MakeAvailable(tbb)

unset(TBB_TESTS)