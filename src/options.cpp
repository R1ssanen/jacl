#include "options.hpp"

#include <span>
#include <string_view>

#include "jacldefs.hpp"
#include "log.hpp"

namespace jacl {

    static std::string Trim(const std::string& str, std::string_view whitespace) {
        const auto begin = str.find_first_not_of(whitespace);
        if (begin == std::string::npos) return str;

        const auto end = str.find_last_not_of(whitespace);
        return str.substr(begin, end - begin + 1);
    }

    static std::vector<std::string> Split(std::string_view str, std::string_view delimiter) {
        std::vector<std::string> tokens;
        u64                      last = 0;
        u64                      next = 0;

        while ((next = str.find_first_of(delimiter, last)) != std::string::npos) {
            tokens.emplace_back(str.substr(last, next - last));
            last = next + 1;
        }

        tokens.emplace_back(str.substr(last));
        return tokens;
    }

} // namespace jacl

namespace jacl {

    OptionParser::OptionParser(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {

            Group group = Split(argv[i], "=,");
            if (group.size() == 0) continue;

            std::string group_name = Trim(group.front(), " ");
            group.erase(group.begin());

            for (auto& option : group) {
                option = Trim(option, " ");
                if (!option.empty()) m_groups[group_name].push_back(option);
            }
        }
    }

    OptionParser::GroupRef OptionParser::GetGroup(const std::string& name) const {
        auto group = m_groups.find(name);
        if (group == m_groups.end()) return std::nullopt;
        else
            return std::span(group->second);
    }

    OptionParser::OptionRef
    OptionParser::GetOption(const std::string& group_name, std::string_view option_name) const {
        GroupRef group = GetGroup(group_name);
        if (!group) return std::nullopt;

        auto option = std::find(group->begin(), group->end(), option_name);
        if (option == group->end()) return std::nullopt;
        else
            return *option;
    }

    OptionParser::OptionRef OptionParser::GetOption(const std::string& group_name) const {
        GroupRef group = GetGroup(group_name);
        if (!group || group->size() == 0) return std::nullopt;
        return group->front();
    }

    bool OptionParser::CheckGroup(const std::string& group_name) const {
        GroupRef group = GetGroup(group_name);
        if (!group || group->empty()) return false;
        else
            return true;
    }

    bool
    OptionParser::CheckOption(const std::string& group_name, std::string_view option_name) const {
        OptionRef option = GetOption(group_name, option_name);
        if (!option) return false;
        else
            return true;
    }

    void OptionParser::ListOptions(void) const {
        std::clog << "jacl: build options\n";

        for (auto& [name, options] : m_groups) {
            std::clog << name << ":\n";

            for (auto& option : options) std::clog << std::string(3, ' ') << option << '\n';
        }
    }

} // namespace jacl
