#include "./TextureRepository.h"

namespace fs = std::filesystem;

repository::TextureRepository* repository::TextureRepository::instance = nullptr;

namespace repository {

TextureRepository::TextureRepository() {
  instance = this;
}

TextureRepository::~TextureRepository() {
  for (auto& [handle, texture] : _textures) {
    UnloadTexture(texture);
  }
  instance = nullptr;
}

TextureRepository* TextureRepository::Get() {
  if (!instance) instance = new TextureRepository;

  return instance;
}

std::optional<Texture2D> TextureRepository::get(const std::string& handle) {
  if (_textures.count(handle) > 0)
    return std::make_optional<Texture>(_textures[handle]);
  return std::nullopt;
}

bool TextureRepository::load(const std::string& handle, const fs::path& resource) {
  if (!fs::exists(resource) || !fs::is_regular_file(resource))
    return false;
  _textures[handle] = LoadTexture(resource.string().c_str());
  return true;
}

}  // namespace repository
