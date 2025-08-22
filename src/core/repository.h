#pragma once

#include <vector>
#include "./repository/FontRepository.h"
#include "./repository/TextureRepository.h"

namespace repository {

std::vector<Repository*> InitRepositories() {
  std::vector<Repository*> repositories;
  repositories.emplace_back(FontRepository::Get());
  repositories.emplace_back(TextureRepository::Get());

  return repositories;
}

}  // namespace repository