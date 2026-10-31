#pragma once

#include <asw/asw.h>

#include <string>

namespace textures {

/**
 * @brief Make a texture from image bytes, e.g. a downloaded jpeg
 *
 * @param data - The image file
 * @return asw::Texture - The texture, empty if it can not be read
 */
asw::Texture from_memory(const std::string& data);

/**
 * @brief Make a black texture that fades from clear at the top to solid at the
 * bottom. Stretch it over an area to darken it smoothly.
 *
 * @return asw::Texture - The texture
 */
asw::Texture create_fade();

}  // namespace textures
