#pragma once

#include <raylib.h>
#include <optional>
#include <unordered_map>
#include "./Repository.h"

namespace repository {

class TextureRepository : public Repository {
  static TextureRepository* instance;
  std::unordered_map<std::string, Texture2D> _textures;

  TextureRepository();
  ~TextureRepository();

 public:
  static TextureRepository* Get();

  bool load(const std::string& handle,
            const std::filesystem::path& resource) override;

  std::optional<Texture2D> get(const std::string& handle);
  
};

}  // namespace repository
