#ifndef JACL_FUNC_DECL_HPP_
#define JACL_FUNC_DECL_HPP_

#include <vector>

#include "../jacldefs.hpp"
#include "../token.hpp"
#include "node.hpp"

namespace jacl {

    class NodeStatementFuncDecl {
      public:

        JACL_NODE_DEBUG_METHODS(FuncDecl)

        bool               Parse(class Parser& parser);

        bool               Generate(class Generator& generator) const;

        std::vector<Token> qualifiers;
        Ptr<NodeQualType>  type;
        Token              identifier;
    };

} // namespace jacl

#endif
