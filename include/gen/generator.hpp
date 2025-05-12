#ifndef JACL_GENERATOR_HPP_
#define JACL_GENERATOR_HPP_

#include <optional>
#include <string>

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

        ASM                           Generate(NodeProgram& program);

        template <typename... T> void Emit(Section section, T... Args) {
            std::string& sect = m_sections[section];

            for (const auto& entry : { Args... }) {
                sect.append(entry);
                sect.append(1, '\t');
            }

            sect.append(1, '\n');
        }

      private:

        std::unordered_map<Section, std::string> m_sections;
    };

} // namespace jacl

#endif
