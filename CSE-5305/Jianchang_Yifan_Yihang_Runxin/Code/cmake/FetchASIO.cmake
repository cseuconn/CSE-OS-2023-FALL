include(FetchContent)
set(FETCHCONTENT_QUIET ON)

message(STATUS "Cloning External Project: ASIO")

FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-28-2
)

FetchContent_MakeAvailable(asio)