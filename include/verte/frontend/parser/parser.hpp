/**
 * @brief Parser class definition.
 * @file parser.hpp
 */

#ifndef VERTE_FRONTEND_PARSER_PARSER_HPP
#define VERTE_FRONTEND_PARSER_PARSER_HPP

#include "verte/frontend/lexer/token.hpp"
#include "verte/frontend/parser/ast.hpp"
#include "verte/utils/logger.hpp"

#include <cstdint>
#include <vector>

/**
 * @namespace verte::nodes
 * @brief AST node classes and related functions & classes.
 */
namespace verte::nodes {
  /**
   * @brief Get the precedence of a token.
   * @param type The token type.
   * @return The precedence.
   */
  [[nodiscard]] static inline int getPrecedence(lexer::Token::Type type) {
    if (tokens::PRECEDENCE.contains(type))
      return tokens::PRECEDENCE.at(type);

    return -1;
  }
} // namespace verte::nodes

/**
 * @namespace verte::nodes
 * @brief AST node classes and related functions & classes.
 */
namespace verte::nodes {
  using namespace verte::lexer;
  using namespace verte::nodes;

  /**
   * @class Parser
   * @brief Handles parsing tokens into an AST.
   */
  class Parser {
  public:
    /**
     * @brief Construct a new Parser.
     * @param tokens The tokens to parse.
     */
    explicit Parser(std::vector<Token> tokens) noexcept
        : index(0), tokens(std::move(tokens)), logger("Parser") {}

    /**
     * @brief Parse the tokens into an AST.
     * @return The AST.
     */
    [[nodiscard]] std::unique_ptr<ProgramNode> parse();

  private:
    /**
     * @brief Parse a single statement.
     * @return The statement node.
     */
    [[nodiscard]] NodePtr parseStmt();

    /**
     * @brief Parse a variable declaration statment.
     */
    [[nodiscard]] NodePtr parseVarDecl();

    /**
     * @brief Parse an assignment statement.
     * @return The parsed assignment statement.
     */
    [[nodiscard]] NodePtr parseAssign();

    /**
     * @brief Parse a function declaration statement.
     */
    [[nodiscard]] NodePtr parseFuncDecl();

    /**
     * @brief Parse prototype for a function declaration.
     * @return The parsed prototype.
     */
    [[nodiscard]] ProtoPtr parseProto();

    /**
     * @brief Parse a parameter list for a function declaration.
     * @return The parsed parameter list.
     */
    [[nodiscard]] std::vector<Parameter> parseParams();

    /**
     * @brief Parse a single parameter for a function declaration.
     * @return The parsed parameter.
     */
    [[nodiscard]] Parameter parseParam();

    /**
     * @brief Parse a type identifier.
     * @return The parsed type identifier.
     */
    [[nodiscard]] TypeInfo parseType();

    /**
     * @brief Parse a return statement.
     * @return The parsed return statement.
     */
    [[nodiscard]] NodePtr parseReturn();

    /**
     * @brief Parse an expression statement.
     * @return The parsed expression statement.
     */
    [[nodiscard]] NodePtr parseExprStmt();

    /**
     * @brief Parse a block of statements.
     * @return The parsed block of statements.
     */
    [[nodiscard]] BlockPtr parseBlock();

    /**
     * @brief Parse an expression.
     * @return The parsed expression.
     */
    [[nodiscard]] NodePtr parseExpr();

    /**
     * @brief Parse a binary expression.
     * @param min The minimun precedence.
     * @return The parsed binary expression.
     */
    [[nodiscard]] NodePtr parseBinary(int min);

    /**
     * @brief Parse a unary expression.
     * @return The parsed unary expression.
     */
    [[nodiscard]] NodePtr parseUnary();

    /**
     * @brief Parse a primary expression.
     * @return The parsed primary expression.
     */
    [[nodiscard]] NodePtr parsePrimary();

    /**
     * @brief Parse a function call.
     * @param callee The function to call.
     * @return The parsed function call.
     */
    [[nodiscard]] NodePtr parseCall(VariablePtr callee);

    /**
     * @brief Get the current token.
     * @return The current token.
     */
    [[nodiscard]] Token currentToken() const;

    /**
     * @brief Get the next token.
     * @return The next token.
     */
    [[nodiscard]] Token nextToken();

    /**
     * @brief Peek at the next token without consuming it.
     * @param offset The offset from the current token. Defaults to 1.
     * @return The next token.
     */
    [[nodiscard]] Token peekToken(size_t offset = 1) const;

    /**
     * @brief Match the current token with the given type.
     * @param type The type to match.
     * @return True if the current token matches the given type, false
     * otherwise.
     */
    [[nodiscard]] bool match(Token::Type type);

    /**
     * @brief Match the current token against a list of types.
     * @param types The list of types to match.
     * @return True if the current token matches any of the given types, false
     * otherwise.
     */
    [[nodiscard]] bool match(const std::initializer_list<Token::Type> types);

    /**
     * @brief Handle a parsing error.
     * @param message The error message.
     */
    [[noreturn]] void error(const std::string &message);

    /**
     * @brief Shortcut for creating nodes.
     * @tparam T The type of node to create.
     * @tparam Args The types of arguments to forward to the constructor.
     * @param args The arguments to forward to the constructor.
     * @return The created node.
     */
    template <typename T, typename... Args> NodePtr create(Args... args);

    size_t index;              /**< The current token index. */
    std::vector<Token> tokens; /**< The tokens to parse. */
    utils::Logger logger;      /**< The logger. */
  };
} // namespace verte::nodes

#endif // VERTE_FRONTEND_PARSER_PARSER_HPP
