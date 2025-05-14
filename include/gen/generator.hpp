#ifndef JACL_GENERATOR_HPP_
#define JACL_GENERATOR_HPP_

#include <format>
#include <optional>
#include <string>

#include "../jacldefs.hpp"
#include "../parse/node.hpp"

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

        void                             EmitPrologue(void);

        void                             EmitEpilogue(void);

        template <typename... Args> void EmitLabel(Section section, Args&&... args) {
            Emit(section, std::forward<Args>(args)..., ":");
        }

        template <typename... Args> void EmitInstruction(Section section, Args&&... args) {
            Emit(section, "", std::forward<Args>(args)...);
        }

        template <typename... Args> void EmitComment(Section section, Args&&... args) {
#if 1
            Emit(section, "\n;", std::forward<Args>(args)...);
#endif
        }

      private:

        std::unordered_map<Section, std::string> m_sections;
        static constexpr u32                     m_column_width = 10;

        template <typename... Args> void         Emit(Section section, Args&&... args) {
            std::string& section_source = m_sections[section];
            auto         entries        = std::array{ args... };

            std::for_each(entries.begin(), entries.end() - 1, [&section_source](const auto& entry) {
                section_source.append(std::format("{:<{}}", entry, Generator::m_column_width));
            });

            section_source.append(entries.back());
            section_source.append(1, '\n');
        }
    };

} // namespace jacl

#endif
