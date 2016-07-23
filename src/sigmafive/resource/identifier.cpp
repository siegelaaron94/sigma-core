#include <sigmafive/resource/identifier.hpp>

#include <functional>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/access.hpp>

#include <sigmafive/util/compile_time_hash.hpp>
#include <sigmafive/util/filesystem.hpp>

namespace sigmafive {
namespace resource {
    bool constexpr_identifier::operator==(const constexpr_identifier& other) const
        noexcept
    {
        return value_ == other.value_;
    }

    bool constexpr_identifier::operator!=(const constexpr_identifier& other) const
        noexcept
    {
        return value_ != other.value_;
    }

    util::hash_type constexpr_identifier::value() const noexcept { return value_; }

    constexpr_identifier::operator util::hash_type() const noexcept
    {
        return value_;
    }

    development_identifier::development_identifier(const char* name)
        : constexpr_identifier(util::compile_time_hash(name))
        , name_(name)
    {
    }

    development_identifier::development_identifier(
        std::string type, boost::filesystem::path path,
        boost::filesystem::path root_directroy)
    {
        // TODO error if path is not in root_directroy
        path = boost::filesystem::absolute(path, root_directroy);
        path = path.replace_extension("");
        auto resource_path = util::path_divergence(root_directroy, path);
        name_ = resource_path.string();
        boost::algorithm::replace_all(name_, "\\", "/");
        name_ = type + "://" + name_;
        value_ = util::compile_time_hash(name_.c_str());
    }

    development_identifier::development_identifier(
        std::string type, boost::filesystem::path path, std::string sub_name,
        boost::filesystem::path root_directroy)
    {
        // TODO error if path is not in root_directroy
        path = boost::filesystem::absolute(path, root_directroy);
        path = path.replace_extension("");
        auto resource_path = util::path_divergence(root_directroy, path);
        name_ = resource_path.string() + ":" + sub_name;
        boost::algorithm::replace_all(name_, "\\", "/");
        name_ = type + "://" + name_;
        value_ = util::compile_time_hash(name_.c_str());
    }

    development_identifier::development_identifier(const std::string& name)
        : constexpr_identifier(util::compile_time_hash(name.c_str()))
        , name_(name)
    {
    }

    std::string development_identifier::name() const { return name_; }

    std::ostream& operator<<(std::ostream& os, const constexpr_identifier& id)
    {
        os << "{"
           << "\"value:\"" << id.value() << "}";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const development_identifier& id)
    {
        os << "{"
           << "\"value:\"" << id.value() << ",\"name:\"" << id.name() << "}";
        return os;
    }
}
}
