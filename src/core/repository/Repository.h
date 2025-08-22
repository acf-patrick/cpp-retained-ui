#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace repository {

class Repository {
protected:
    virtual ~Repository() = default;
public:
    virtual bool load(const std::string& handle,
                      const std::filesystem::path& resource) = 0;

    static void Clear(const std::vector<Repository*>& repositories);
};

}  // namespace repository