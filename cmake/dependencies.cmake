include(FetchContent)

function(fetch_dependency name)
  FetchContent_GetProperties(${name})
  if(NOT ${name}_POPULATED)
    message(STATUS "Fetching and configuring ${name}...")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")

    FetchContent_Populate(${name})
    add_subdirectory(
      ${${name}_SOURCE_DIR}
      ${${name}_BINARY_DIR}
      # we have to set EXCLUDE_FROM_ALL so that dependent targets are not
      # included into the install trees.
      EXCLUDE_FROM_ALL)

    list(POP_BACK CMAKE_MESSAGE_INDENT)
  endif()
endfunction(fetch_dependency)

FetchContent_Declare(
  gp-sdk
  # GIT_REPOSITORY https://github.com/gigperformer/gp-sdk.git
  # GIT_TAG 756ecd249f1ff00f3d3a5955769408f74c9fa453 # sdk v60
  GIT_REPOSITORY https://github.com/WidnerM/gp-sdk.git
  # GIT_TAG 1e62325f8ec5b36a94f3cfe0b5395f325e149479
  GIT_TAG 1ba74eefabaa25d15542a72db2d66d234c1602f4
)
fetch_dependency(gp-sdk)

FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 2f980209cc4091a4490bb1bafc5d530f16834e58 # v6.1.6
)
fetch_dependency(juce)
