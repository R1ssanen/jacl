#include "generator.hpp"

#include <string>

#include "lang/program.hpp"

namespace jacl {

    Generator::Generator(void) {
        Emit(Section::BSS, "section", ".bss");
        Emit(Section::DATA, "section", ".data");
        Emit(Section::RODATA, "section", ".rodata");
        Emit(Section::TEXT, "section", ".text");
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
