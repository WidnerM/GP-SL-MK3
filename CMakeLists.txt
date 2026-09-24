# some required definitions to compile properly
cmake_minimum_required(VERSION 3.21)
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.3" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Let's choose a name and version for the extension. Change the name to your
# liking. The version should be a string like "1.0".
set(PROJECT_NAME "GP-SLMK3") 
set (PROJECT_VERSION "1.1.0")
set (PROJECT_TITLE "Novation SL-MK3 Extension")
set (PROJECT_DESCR "GP Extension for Novation SL-MK3 controllers")
string(TIMESTAMP TODAY "%d/%m/%Y")
set (PROJECT_BUILD_DATE ${TODAY})
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})


# in the dependencies directory we bring in the GP SDK and whatever is required of juce
include("${CMAKE_CURRENT_LIST_DIR}/cmake/dependencies.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/juce-shared-library.cmake")

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/Version.h @ONLY)

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
                          "${CMAKE_CURRENT_LIST_DIR}/src/General_Utils.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/src/DelayCallback.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE gigperformer::sdk::cpp
                                    juce::juce_gui_basics juce::juce_core juce::juce_events juce::juce_graphics)

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
