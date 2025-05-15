#ifndef JACL_GENERATOR_HPP_
#define JACL_GENERATOR_HPP_

#include <format>
#include <optional>
#include <string>
#include <unordered_map>

#include "jacldefs.hpp"
#include "lang/node.hpp"

namespace jacl {

    enum class Section {
        TEXT,
        BSS,
        DATA,
        RODATA,
    };

    class Generator {
      public:

        using ASM = std::optional<std::string>;

        Generator(void);

        ASM                              Generate(NodeProgram& program);

        template <typename... Args> void Emit(Section section, Args&&... args) {
            EmitFormatted(section, "", std::forward<Args>(args)...);
        }

        void EmitLabel(Section section, const char* label) {
            m_sections[section].append(std::format("{}:\n", label));
        }

        template <typename... Args> void EmitComment(Section section, Args&&... args) {
            EmitFormatted(section, "\n;", std::forward<Args>(args)...);
        }

        template <typename... Args> void EmitFormatted(Section section, Args&&... args) {
            std::string& section_source = m_sections[section];
            auto         entries        = std::array{ args... };

            std::for_each(entries.begin(), entries.end() - 1, [&section_source](const auto& entry) {
                section_source.append(std::format("{:<{}}", entry, Generator::m_column_width));
            });

            section_source.append(entries.back());
            section_source.append(1, '\n');
        }

      private:

        std::unordered_map<Section, std::string> m_sections;
        static constexpr u32                     m_column_width = 10;
    };

} // namespace jacl

#endif
