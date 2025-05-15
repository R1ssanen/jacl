#ifndef JACL_QUAL_TYPE_HPP_
#define JACL_QUAL_TYPE_HPP_

#include "../jacldefs.hpp"
#include "../token.hpp"
#include "node.hpp"

namespace jacl {

    class NodeQualType {
      public:

        JACL_NODE_DEBUG_METHODS(QualType)

        bool               Parse(class Parser& parser);

        bool               Generate(class Generator& generator) const;

        std::vector<Token> qualifiers;
        Token              base_type;
    };

} // namespace jacl

#endif
