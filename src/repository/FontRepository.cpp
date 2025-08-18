#include "./FontRepository.h"

namespace fs = std::filesystem;

repository::FontRepository* repository::FontRepository::instance = nullptr;

namespace repository {

FontRepository::FontRepository() {
  instance = this;
}

FontRepository::~FontRepository() {
  for (auto& [handle, font] : _fonts) {
    UnloadFont(font);
  }
}

FontRepository* FontRepository::Get() {
  if (!instance) instance = new FontRepository;

  return instance;
}

std::optional<Font> FontRepository::get(const std::string& handle) {
  if (_fonts.count(handle) > 0)
    return std::make_optional<Font>(_fonts[handle]);
  return std::nullopt;
}

bool FontRepository::load(const std::string& handle, const fs::path& resource) {
  if (!fs::exists(resource) || !fs::is_regular_file(resource))
    return false;
  _fonts[handle] = LoadFont(resource.string().c_str());
  return true;
}

}  // namespace repository
