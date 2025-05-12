#include "gen/generator.hpp"

namespace jacl {

    Generator::Generator(void) {
        Emit(Section::BSS, "section", ".bss\n");
        Emit(Section::DATA, "section", ".data\n");
        Emit(Section::RODATA, "section", ".rodata\n");

        Emit(Section::TEXT, "section", ".text\n");
        Emit(Section::TEXT, "global", "_start");
        Emit(Section::TEXT, "_start:");
    }

    Generator::ASM Generator::Generate(NodeProgram& program) {

        if (!program.Generate(*this)) {
            return std::nullopt;
        } else {

            std::string source;
            source.append(m_sections[Section::DATA]);
            source.append(m_sections[Section::RODATA]);
            source.append(m_sections[Section::BSS]);
            source.append(m_sections[Section::TEXT]);

            return source;
        }
    }

} // namespace jacl
