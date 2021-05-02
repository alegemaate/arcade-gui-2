cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project (arcade-gui-2 VERSION 1.0.0 LANGUAGES CXX)

# Add sources
file(GLOB_RECURSE SOURCES ${CMAKE_BINARY_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_BINARY_DIR}/src/*.h)

# Create lib
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add compile options
target_compile_options(${PROJECT_NAME} PRIVATE -O3 -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-sign-compare)

# Add library include
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/lib/include)

# Find libs
find_library(ALLEGRO_LIBRARY NAMES allegro REQUIRED)
find_library(ALLEGRO_ACODEC_LIBRARY NAMES allegro_acodec REQUIRED)
find_library(ALLEGRO_AUDIO_LIBRARY NAMES allegro_audio REQUIRED)
find_library(ALLEGRO_COLOR_LIBRARY NAMES allegro_color REQUIRED)
find_library(ALLEGRO_DIALOG_LIBRARY NAMES allegro_dialog REQUIRED)
find_library(ALLEGRO_FONT_LIBRARY NAMES allegro_font REQUIRED)
find_library(ALLEGRO_IMAGE_LIBRARY NAMES allegro_image REQUIRED)
find_library(ALLEGRO_MAIN_LIBRARY NAMES allegro_main REQUIRED)
find_library(ALLEGRO_PRIMITIVES_LIBRARY NAMES allegro_primitives REQUIRED)
find_library(ALLEGRO_TTF_LIBRARY NAMES allegro_ttf REQUIRED)
find_library(BOX2D_LIBRARY NAMES Box2D REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Link Libs
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_ACODEC_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_AUDIO_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_COLOR_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_DIALOG_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_FONT_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_IMAGE_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_MAIN_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_PRIMITIVES_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ALLEGRO_TTF_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${BOX2D_LIBRARY})
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)