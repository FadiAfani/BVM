include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)

FetchContent_MakeAvailable(Catch2)

file(GLOB SOURCES "*.cpp")

add_executable(tests ${SOURCES})

# Link Catch2 to the test executable
target_link_libraries(tests Catch2::Catch2)

enable_testing()

# Register test
add_test(NAME TestAll COMMAND tests)


