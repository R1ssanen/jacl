#ifndef JACL_FUNC_DEF_HPP_
#define JACL_FUNC_DEF_HPP_

#include "../jacldefs.hpp"
#include "node.hpp"

namespace jacl {

    class NodeStatementFuncDef {
      public:

        JACL_NODE_DEBUG_METHODS(FuncDef)

        bool                       Parse(class Parser& parser);

        bool                       Generate(class Generator& generator) const;

        Ptr<NodeStatementFuncDecl> declaration;
        Ptr<NodeScope>             scope;
    };

} // namespace jacl

#endif
