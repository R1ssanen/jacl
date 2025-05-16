#ifndef JACL_OPTIONS_HPP_
#define JACL_OPTIONS_HPP_

#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "jacldefs.hpp"

namespace jacl {

    class OptionParser {
      public:

        using Group = std::vector<std::string>;

        OptionParser(int argc, char** argv);

        const Group*     GetGroup(const std::string& group) const;

        std::string_view GetOption(const std::string& group, std::string_view option) const;

        std::string_view GetOption(const std::string& group) const;

        bool             GroupSet(const std::string& group_name) const;

        bool             OptionSet(const std::string& group, std::string_view option_name) const;

        void             ListOptions(void) const;

      private:

        std::unordered_map<std::string, Group> m_groups;
    };

} // namespace jacl

#endif
