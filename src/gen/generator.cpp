#include "gen/generator.hpp"

namespace jacl {

    Generator::Generator(void) {
        Emit(Section::BSS, "section", ".bss");
        Emit(Section::DATA, "section", ".data");
        Emit(Section::RODATA, "section", ".rodata");

        Emit(Section::TEXT, "section", ".text");
        Emit(Section::TEXT, "global", "_start");
        EmitLabel(Section::TEXT, "_start");
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

    void Generator::EmitPrologue(void) {
        EmitInstruction(Section::TEXT, "push", "rbp");
        EmitInstruction(Section::TEXT, "mov", "rbp,", "rsp");
        EmitInstruction(Section::TEXT, "sub", "rsp,", "16");
    }

    void Generator::EmitEpilogue(void) {
        EmitInstruction(Section::TEXT, "add", "rsp,", "16");
        EmitInstruction(Section::TEXT, "leave");
        EmitInstruction(Section::TEXT, "ret");
    }

} // namespace jacl
