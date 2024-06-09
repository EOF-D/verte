/**
 * @brief Parser implementation.
 * @file parser.cpp
 */

#include "verte/frontend/parser/parser.hpp"
#include "verte/errors.hpp"

namespace verte::nodes {
  [[nodiscard]] std::unique_ptr<ProgramNode> Parser::parse() {
    // PROGRAM -> STMT*
    std::vector<NodePtr> body;

    // Keep parsing until we reach the EOF.
    // This will be the body of the module.
    while (!currentToken().is(Token::Type::EOS))
      body.push_back(parseStmt());

    return std::make_unique<ProgramNode>(std::move(body));
  }

  [[nodiscard]] NodePtr Parser::parseStmt() {
    auto token = currentToken();
    auto next = peekToken();

    // Check if the current token is a variable declaration.
    if ((token.is(Token::Type::IDENTIFIER) || token.is(Token::Type::CONST)) &&
        (next.is(Token::Type::IDENTIFIER) || next.is(Token::Type::COLON)))
      return parseVarDecl();

    // Check if the current token is a variable assignment.
    else if (token.is(Token::Type::IDENTIFIER) && next.is(Token::Type::EQUAL))
      return parseAssign();

    // Check if the current token is a block.
    else if (token.is(Token::Type::LBRACE))
      return parseBlock();

    // Check if the current token is a function/prototype declaration.
    else if (token.is(Token::Type::FN))
      return parseFuncDecl();

    // Check if the current token is a return statement.
    else if (token.is(Token::Type::RETURN))
      return parseReturn();

    // Default to an expression statement.
    return parseExprStmt();
  }

  [[nodiscard]] NodePtr Parser::parseVarDecl() {
    // VAR_DECL -> (CONST)? IDENTIFIER ':' TYPE '=' EXPR ';'
    bool isConst = false;
    if (match(Token::Type::CONST))
      isConst = true;

    auto ident = currentToken();

    if (!match(Token::Type::IDENTIFIER))
      error("Expected an identifier for variable declaration.");

    if (!match(Token::Type::COLON))
      error("Expected a `:` after the identifier.");

    auto type = parseType();

    if (!match(Token::Type::ASSIGN))
      error("Expected an `=` after the type.");

    auto expr = parseExpr();
    if (!match(Token::Type::SEMICOLON))
      error("Expected a `;` after the expression.");

    auto value = ident.getValue();
    return create<VarDeclNode>(value, type, std::move(expr), isConst);
  }

  [[nodiscard]] NodePtr Parser::parseAssign() {
    auto ident = currentToken();

    if (!match(Token::Type::IDENTIFIER))
      error("Expected an identifier for variable assignment.");

    if (!match(Token::Type::EQUAL))
      error("Expected an `=` after the identifier.");

    auto expr = parseExpr();
    if (!match(Token::Type::SEMICOLON))
      error("Expected a `;` after the expression.");

    return create<AssignNode>(ident.getValue(), std::move(expr));
  }

  [[nodiscard]] NodePtr Parser::parseFuncDecl() {
    // FUNC_DECL -> FN IDENTIFIER '(' PARAMS ')' '->' TYPE (';' | '{' STMT* '}')
    if (!match(Token::Type::FN))
      error("Expected a `fn` for the function declaration.");

    auto proto = parseProto();

    if (match(Token::Type::SEMICOLON))
      return proto;

    else if (currentToken().is(Token::Type::LBRACE))
      return create<FuncDeclNode>(std::move(proto), parseBlock());

    error("Expected a `;` or `{` after the function prototype.");
  }

  [[nodiscard]] ProtoPtr Parser::parseProto() {
    // PROTO -> IDENTIFIER '(' PARAMS ')' '->' TYPE
    auto ident = currentToken();
    if (!match(Token::Type::IDENTIFIER))
      error("Expected an identifier for the function name.");

    if (!match(Token::Type::LPAREN))
      error("Expected a `(` after the function name.");

    std::vector<Parameter> params = parseParams();

    if (!currentToken().is(Token::Type::MINUS) &&
        !peekToken().is(Token::Type::GREATER)) {
      error("Expected a `-> return type` after the parameters.");
    }

    index += 2; // Skip the `->` token.
    return std::make_unique<ProtoNode>(ident.getValue(), params, parseType());
  }

  [[nodiscard]] std::vector<Parameter> Parser::parseParams() {
    // PARAMS -> (PARAM (',' PARAM)*)?
    std::vector<Parameter> params;

    // Parse until we reach the closing parenthesis.
    if (!currentToken().is(Token::Type::RPAREN))
      do {
        params.push_back(parseParam());
        if (currentToken().is(Token::Type::RPAREN))
          break;

        if (!match(Token::Type::COMMA))
          error("Expected a `,` or `)` after the parameter.");
      } while (true);

    // Consume the closing parenthesis.
    if (!match(Token::Type::RPAREN))
      error("Expected a `)` after the parameter list.");

    return params;
  }

  [[nodiscard]] Parameter Parser::parseParam() {
    // PARAM -> IDENTIFIER ':' TYPE
    auto ident = currentToken();
    if (!match(Token::Type::IDENTIFIER))
      error("Expected an identifier for the parameter name.");

    if (!match(Token::Type::COLON))
      error("Expected a `:` after the parameter name.");

    auto type = parseType();
    return Parameter(ident.getValue(), type);
  }

  [[nodiscard]] TypeInfo Parser::parseType() {
    // TYPE -> IDENTIFIER
    auto token = currentToken();
    if (!match(Token::Type::IDENTIFIER))
      error("Expected a type identifier.");

    return TypeInfo(TypeInfo::toEnum(token.getValue()), token.getValue());
  }

  [[nodiscard]] NodePtr Parser::parseReturn() {
    // RETURN_STMT -> RETURN EXPR ';'
    if (!match(Token::Type::RETURN))
      error("Expected a `return` for the return statement.");

    auto expr = parseExpr();
    if (!match(Token::Type::SEMICOLON))
      error("Expected a `;` after the expression.");

    return create<ReturnNode>(std::move(expr));
  }

  [[nodiscard]] NodePtr Parser::parseExprStmt() {
    // EXPR_STMT -> EXPR ';'
    auto expr = parseExpr();
    if (!match(Token::Type::SEMICOLON))
      error("Expected a `;` after the expression.");

    return expr;
  }

  [[nodiscard]] BlockPtr Parser::parseBlock() {
    // BLOCK -> '{' STMT* '}'
    std::vector<NodePtr> body;
    if (!match(Token::Type::LBRACE))
      error("Expected a `{` to start a block.");

    // Parse until we reach the closing brace.
    while (!match(Token::Type::RBRACE))
      body.push_back(parseStmt());

    return std::make_unique<BlockNode>(std::move(body));
  }

  [[nodiscard]] NodePtr Parser::parseExpr() {
    // EXPR -> BINARY
    return parseBinary(0);
  }

  [[nodiscard]] NodePtr Parser::parseBinary(int min) {
    // BINARY -> UNARY (BINARY_OP UNARY)*
    auto lhs = parseUnary();
    int currentPrec = getPrecedence(currentToken().type);

    while (currentPrec >= min) {
      if (currentToken().isOneOf(tokens::BINARY_OPERATOR_TYPES)) {
        auto op = currentToken();
        index++; // Consume the operator.

        // Parse the right-hand side of the binary expression with higher
        // precedence.
        auto rhs = parseBinary(currentPrec + 1);
        lhs = create<BinaryNode>(std::move(lhs), std::move(rhs), op.getValue());
        currentPrec = getPrecedence(currentToken().type);
      } else {
        break;
      }
    }

    return lhs;
  }

  [[nodiscard]] NodePtr Parser::parseUnary() {
    // UNARY -> (UNARY_OP UNARY | PRIMARY)
    if (currentToken().isOneOf(tokens::UNARY_OPERATOR_TYPES)) {
      auto op = currentToken();
      index++; // Consume the operator.

      auto expr = parseUnary();
      return create<UnaryNode>(std::move(expr), op.getValue());
    }

    // If it's not a unary operator, parse the primary expression.
    return parsePrimary();
  }

  [[nodiscard]] NodePtr Parser::parsePrimary() {
    // PRIMARY -> LITERAL | IDENTIFIER | '(' EXPR ')'
    auto token = currentToken();

    // Check for literals.
    if (match(Token::Type::STRING)) {
      TypeInfo type(TypeInfo::DataType::STRING);
      return create<LiteralNode>(token.getValue(), type);
    }

    else if (match(Token::Type::NUMBER)) {
      TypeInfo type(TypeInfo::DataType::INTEGER);
      return create<LiteralNode>(token.getValue(), type);
    }

    else if (match(Token::Type::TRUE) || match(Token::Type::FALSE)) {
      TypeInfo type(TypeInfo::DataType::BOOL);
      return create<LiteralNode>(token.getValue(), type);
    }

    else if (match(Token::Type::IDENTIFIER)) {
      TypeInfo type(TypeInfo::DataType::UNKNOWN);
      auto ident = std::make_unique<VariableNode>(token.getValue());

      // Check if it's a function call.
      if (currentToken().is(Token::Type::LPAREN))
        return parseCall(std::move(ident));

      // Otherwise, it's just an identifier.
      return ident;
    }

    // Check for parenthesized expressions.
    else if (match(Token::Type::LPAREN)) {
      auto expr = parseExpr();
      if (!match(Token::Type::RPAREN))
        error("Expected a `)` after the expression.");

      return expr;
    }

    error("Expected a primary expression.");
    return nullptr;
  }

  [[nodiscard]] NodePtr Parser::parseCall(VariablePtr callee) {
    // CALL -> IDENTIFIER '(' (EXPR (',' EXPR)*)? ')'
    std::vector<NodePtr> args;
    if (!match(Token::Type::LPAREN))
      error("Expected a `(` after the function identifier.");

    // Parse until we reach the closing parenthesis.
    if (!currentToken().is(Token::Type::RPAREN))
      do {
        args.push_back(parseExpr());
        if (currentToken().is(Token::Type::RPAREN))
          break;

        if (!match(Token::Type::COMMA))
          error("Expected a `,` or `)` after the argument.");

        // Check for trailing commas.
        else if (match(Token::Type::COMMA) &&
                 peekToken().is(Token::Type::RPAREN))
          error("Expected an argument after the comma.");
      } while (true);

    if (!match(Token::Type::RPAREN))
      error("Expected a `)` after the argument list.");

    return create<CallNode>(std::move(callee), std::move(args));
  }

  [[nodiscard]] Token Parser::currentToken() const {
    // If we're at the end of the tokens, return EOF.
    if (index >= tokens.size())
      return tokens.back();

    return tokens[index];
  }

  [[nodiscard]] Token Parser::nextToken() {
    index++;

    Token token = currentToken();
    if (token.is(Token::Type::EOS)) {
      index = tokens.size(); // Make sure index is set to size if EOF.
      return tokens.back();
    }

    return token;
  }

  [[nodiscard]] Token Parser::peekToken(size_t offset) const {
    if (index + offset >= tokens.size())
      return tokens.back();

    return tokens[index + offset];
  }

  [[nodiscard]] bool Parser::match(Token::Type type) {
    if (currentToken().is(type)) {
      index++;
      return true;
    }

    return false;
  }

  [[nodiscard]] bool
  Parser::match(const std::initializer_list<Token::Type> types) {
    if (currentToken().isOneOf(types)) {
      index++;
      return true;
    }

    return false;
  }

  [[noreturn]] void Parser::error(const std::string &message) {
    auto [line, column] = currentToken().meta;
    std::string error = std::format("{}:{}: {}", line, column, message);

    logger.error(error);
    throw errors::ParserError(error, line, column);
  }

  template <typename T, typename... Args> NodePtr Parser::create(Args... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }
} // namespace verte::nodes
