#ifndef JACL_OPTIONS_HPP_
#define JACL_OPTIONS_HPP_

#include <optional>
#include <span>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "jacldefs.hpp"

namespace jacl {

    class OptionParser {
      public:

        using Group     = std::vector<std::string>;
        using GroupRef  = std::optional<std::span<const std::string>>;
        using OptionRef = std::optional<std::string_view>;

        OptionParser(int argc, char** argv);

        GroupRef  GetGroup(const std::string& group_name) const;

        OptionRef GetOption(const std::string& group_name, std::string_view option_name) const;

        OptionRef GetOption(const std::string& group_name) const;

        bool      CheckGroup(const std::string& group_name) const;

        bool      CheckOption(const std::string& group_name, std::string_view option_name) const;

        void      ListOptions(void) const;

      private:

        std::unordered_map<std::string, Group> m_groups;
    };

} // namespace jacl

#endif
