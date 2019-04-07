#ifndef SIGMA_ENGINE_FILESYSTEM_FILESYSTEM_HPP
#define SIGMA_ENGINE_FILESYSTEM_FILESYSTEM_HPP

#include <sigma/config.hpp>

#include <cereal/cereal.hpp>

#include <filesystem>

namespace sigma {
namespace filesystem {
    bool SIGMA_API contains_file(std::filesystem::path directory, std::filesystem::path file);

    std::filesystem::path SIGMA_API make_relative(std::filesystem::path directory, std::filesystem::path file);

    bool SIGMA_API is_hidden(const std::filesystem::path& path);
}
}

namespace std {
template <>
struct hash<std::filesystem::path> {
    size_t operator()(const std::filesystem::path& p) const
    {
        return std::filesystem::hash_value(p);
    }
};
}

namespace cereal {
    template <class Archive>
    void serialize(Archive& ar, std::filesystem::path& p)
    {
        std::filesystem::path::string_type s;
        if (Archive::is_saving::value)
            s = p.string();
        ar(cereal::make_nvp("string", s));
        if (Archive::is_loading::value)
            p = s;
    }
}

#endif // SIGMA_ENGINE_FILESYSTEM_FILESYSTEM_HPP
