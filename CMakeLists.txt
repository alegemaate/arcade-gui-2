cmake_minimum_required(VERSION 3.24)

project(arcade-gui-2 VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target)

include(cmake/get_cpm.cmake)
# asw pulls in SDL_mixer, which needs a system opusfile for Opus. Not needed here.
set(SDLMIXER_OPUS OFF CACHE INTERNAL "")
CPMAddPackage("gh:adsgames/asw@0.12.4")
CPMAddPackage("gh:nlohmann/json@3.11.3")

# HTTPS client for the A.D.S. Games API. Secure Transport on macOS, OpenSSL
# elsewhere, the same setup as play-sdk.
set(USE_TLS ON CACHE INTERNAL "")
set(USE_ZLIB OFF CACHE INTERNAL "")
set(IXWEBSOCKET_INSTALL OFF CACHE INTERNAL "")
if(WIN32)
  set(USE_OPEN_SSL ON CACHE INTERNAL "")
  set(OPENSSL_USE_STATIC_LIBS ON)
endif()
CPMAddPackage(
  NAME ixwebsocket
  URL https://github.com/machinezone/IXWebSocket/archive/refs/tags/v12.0.1.tar.gz
)

# Unzips downloaded games. The release archive is the single file amalgamation.
CPMAddPackage(
  NAME miniz
  URL https://github.com/richgel999/miniz/releases/download/3.0.2/miniz-3.0.2.zip
  DOWNLOAD_ONLY YES
)
add_library(miniz STATIC ${miniz_SOURCE_DIR}/miniz.c)
target_include_directories(miniz SYSTEM PUBLIC ${miniz_SOURCE_DIR})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
target_link_libraries(${PROJECT_NAME} PRIVATE
  asw
  nlohmann_json::nlohmann_json
  ixwebsocket::ixwebsocket
  miniz
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/assets/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)
