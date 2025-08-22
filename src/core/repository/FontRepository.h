#pragma once

#include <raylib.h>
#include <optional>
#include <unordered_map>
#include "./Repository.h"

namespace repository {

class FontRepository : public Repository {
  static FontRepository* instance;
  std::unordered_map<std::string, Font> _fonts;

  FontRepository();
  ~FontRepository();

 public:
  static FontRepository* Get();

  bool load(const std::string& handle,
            const std::filesystem::path& resource) override;

  std::optional<Font> get(const std::string& handle);

};

}  // namespace repository
