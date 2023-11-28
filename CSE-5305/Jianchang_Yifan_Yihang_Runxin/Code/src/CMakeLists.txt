add_library(policy)
add_library(scheduler)
add_library(utils)

target_sources(policy
        PUBLIC
        FILE_SET CXX_MODULES FILES
        policy.cppm
)

target_sources(
        utils
        PUBLIC
        FILE_SET CXX_MODULES FILES
        utils.cppm
)

target_sources(scheduler
        PUBLIC
        FILE_SET CXX_MODULES FILES
        scheduler.cppm
)

target_include_directories(scheduler PUBLIC ${asio_SOURCE_DIR}/asio/include)

target_link_libraries(scheduler PRIVATE policy utils spdlog::spdlog)

add_executable(main main.cpp)
target_include_directories(main PRIVATE ${asio_SOURCE_DIR}/asio/include)

# Link the libraries
target_link_libraries(main PRIVATE policy scheduler utils spdlog::spdlog argparse::argparse)