#include "./Repository.h"

namespace repository {
void Repository::Clear(const std::vector<Repository*>& repositories) {
  for (auto repository : repositories)
    delete repository;
}
}  // namespace repository