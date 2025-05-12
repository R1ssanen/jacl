#ifndef JACL_NODE_HPP_
#define JACL_NODE_HPP_

#include <variant>
#include <vector>

#include "../jacldefs.hpp"
#include "../token.hpp"

#define JACL_NODE_VIRTUALS_NO_GEN(kind)                                                            \
                                                                                                   \
  public:                                                                                          \
                                                                                                   \
    inline NodeKind         GetKind(void) const override { return NodeKind::kind; }                \
    inline std::string_view GetDebugName(void) const override { return "Node::" #kind; }           \
    void                    DebugPrint(u64 level) const override;                                  \
    bool                    Parse(class jacl::Parser& parser) override;

#define JACL_NODE_VIRTUALS(kind)                                                                   \
    JACL_NODE_VIRTUALS_NO_GEN(kind)                                                                \
    bool Generate(class jacl::Generator& generator) const override;

namespace jacl {

    enum class NodeKind {
        PROGRAM,
        LITERAL,
        QUALIFIED_TYPE,
        SCOPE,
        STATEMENT,
        STATEMENT_FUNC_DEF,
        STATEMENT_RETURN,
    };

    class Node {
      public:

        virtual ~Node()                                      = default;

        virtual NodeKind         GetKind(void) const         = 0;

        virtual std::string_view GetDebugName(void) const    = 0;

        virtual void             DebugPrint(u64 level) const = 0;

        virtual bool             Parse(class Parser& parser) = 0;

        virtual bool             Generate(class Generator&) const { return true; }
    };

} // namespace jacl

namespace jacl {

    class NodeProgram;
    class NodeLiteral;
    class NodeScope;
    class NodeQualifiedType;
    class NodeStatement;
    class NodeStatementFuncDefinition;
    class NodeStatementReturn;

} // namespace jacl

namespace jacl {

    class NodeProgram : public Node {
      private:

        std::vector<Owning<NodeStatement>> m_statements;

        JACL_NODE_VIRTUALS(PROGRAM)
    };

    class NodeQualifiedType : public Node {
      private:

        std::vector<Token> m_qualifiers;
        Token              m_base_type;

        JACL_NODE_VIRTUALS_NO_GEN(QUALIFIED_TYPE)
    };

    class NodeStatement : public Node {
      private:

        std::variant<Owning<NodeStatementFuncDefinition>, Owning<NodeStatementReturn>> m_statement;

        JACL_NODE_VIRTUALS(STATEMENT)
    };

    class NodeStatementReturn : public Node {
      private:

        Owning<NodeLiteral> m_value;

        JACL_NODE_VIRTUALS(STATEMENT_RETURN)
    };

    class NodeStatementFuncDefinition : public Node {
      private:

        Token                     m_identifier;
        Owning<NodeQualifiedType> m_type;
        Owning<NodeScope>         m_scope;

        JACL_NODE_VIRTUALS(STATEMENT_FUNC_DEF)
    };

    class NodeLiteral : public Node {
      private:

        std::variant<i64, f64, std::string> m_value;

        JACL_NODE_VIRTUALS(LITERAL)
    };

    class NodeScope : public Node {
      private:

        std::vector<Owning<NodeStatement>> m_statements;

        JACL_NODE_VIRTUALS(SCOPE)
    };

} // namespace jacl

#endif
