cmake_minimum_required(VERSION 3.21)

# Let's choose a name and version for the extension. Change the name to your
# liking. The version should be a string like "1.0".
set(PROJECT_NAME "GP-SLMK3") # Change this to your liking
project(${PROJECT_NAME} VERSION 1.0)

# Import the SDK
include(FetchContent)
FetchContent_Declare(
  gp-sdk
  GIT_REPOSITORY https://github.com/gigperformer/gp-sdk.git
  GIT_TAG 3d4fa49c3d6f96851961893ce51896b4450e59ae # v46
)
FetchContent_MakeAvailable(gp-sdk)

# Define our library including sources, include directories and dependencies
add_library(${PROJECT_NAME} SHARED)
target_sources(
  ${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/LibMain.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/src/LibMain.h"
                          "${CMAKE_CURRENT_LIST_DIR}/src/Display.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/src/Songlist.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/src/Inputs.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/src/Knobs.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/src/Buttons.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/src/General_Utils.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE gigperformer::sdk::cpp)
target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Language options: this will be a pure C++20 project
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS NO)

# Install the extension on the development machine
install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION "${GIG_PERFORMER_EXTENSIONS_DIRECTORY}"
  RUNTIME DESTINATION "${GIG_PERFORMER_EXTENSIONS_DIRECTORY}")
