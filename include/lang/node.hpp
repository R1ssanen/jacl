#ifndef JACL_NODE_HPP_
#define JACL_NODE_HPP_

#include <string_view>

#include "../jacldefs.hpp"

#define JACL_NODE_DEBUG_METHODS(name)                                                              \
    inline constexpr std::string_view GetDebugName(void) const { return "Node::" #name; }          \
    void                              DebugPrint(u32 level = 0, u8 indent = 3) const;

namespace jacl {

    class NodeProgram;

    class NodeRootStatement;
    class NodeStatement;
    class NodeStatementFuncDecl;
    class NodeStatementFuncDef;
    class NodeStatementReturn;
    class NodeStatementFuncCall;

    class NodeScope;
    class NodeQualType;
    class NodeLiteral;

} // namespace jacl

#endif
