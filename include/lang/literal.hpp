#ifndef JACL_LITERAL_HPP_
#define JACL_LITERAL_HPP_

#include <variant>

#include "../jacldefs.hpp"
#include "node.hpp"

namespace jacl {

    class NodeLiteral {
      public:

        JACL_NODE_DEBUG_METHODS(Literal)

        bool                                   Parse(class Parser& parser);

        bool                                   Generate(class Generator& generator) const;

        std::variant<std::monostate, i64, f64> value;
        std::string                            string;
    };

} // namespace jacl

#endif
