/**
 * @file parser.cpp
 * @brief Parser implementation.
 */

#include "excerpt/parser/parser.hpp"

namespace excerpt {
  [[nodiscard]] std::unique_ptr<ProgramNode> Parser::parse() {
    // PROGRAM -> STMT*
    std::vector<NodePtr> body;

    // Keep parsing until we reach the EOF.
    // This will be the body of the module.
    while (!current().is(Token::Type::END))
      body.push_back(parseStmt());

    return std::make_unique<ProgramNode>(std::move(body));
  }

  [[nodiscard]] NodePtr Parser::parseStmt() {
    auto token = current();

    // Check if the current token is a variable declaration.
    if (token.is(Token::Type::IDENTIFIER) && peek().is(Token::Type::COLON))
      return parseVarDecl();

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
    // VAR_DECL -> IDENTIFIER ':' TYPE '=' EXPR ';'
    auto ident = current();

    if (!match(Token::Type::IDENTIFIER))
      error("Expected an identifier for variable declaration.");

    if (!match(Token::Type::COLON))
      error("Expected a `:` after the identifier.");

    auto type = parseType();

    if (!match(Token::Type::EQUAL))
      error("Expected an `=` after the type.");

    auto expr = parseExpr();
    if (!match(Token::Type::SEMICOLON))
      error("Expected a `;` after the expression.");

    return create<VarDeclNode>(ident.value, type, std::move(expr));
  }

  [[nodiscard]] NodePtr Parser::parseFuncDecl() {
    // FUNC_DECL -> FN IDENTIFIER '(' PARAMS ')' '->' TYPE (';' | '{' STMT* '}')
    if (!match(Token::Type::FN))
      error("Expected a `fn` for the function declaration.");

    auto proto = parseProto();

    if (match(Token::Type::SEMICOLON))
      return proto;

    else if (current().is(Token::Type::LBRACE))
      return create<FuncDeclNode>(std::move(proto), parseBlock());

    error("Expected a `;` or `{` after the function prototype.");
  }

  [[nodiscard]] NodePtr Parser::parseProto() {
    // PROTO -> IDENTIFIER '(' PARAMS ')' '->' TYPE
    auto ident = current();
    if (!match(Token::Type::IDENTIFIER))
      error("Expected an identifier for the function name.");

    if (!match(Token::Type::LPAREN))
      error("Expected a `(` after the function name.");

    std::vector<Parameter> params = parseParams();

    if (!current().is(Token::Type::MINUS) && !peek().is(Token::Type::GREATER)) {
      error("Expected a `-> return type` after the parameters.");
    }

    index += 2; // Skip the `->` token.
    return create<ProtoNode>(ident.value, params, parseType());
  }

  [[nodiscard]] std::vector<Parameter> Parser::parseParams() {
    // PARAMS -> (PARAM (',' PARAM)*)?
    std::vector<Parameter> params;

    // Parse until we reach the closing parenthesis.
    if (!current().is(Token::Type::RPAREN))
      do {
        params.push_back(parseParam());
        if (current().is(Token::Type::RPAREN))
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
    auto ident = current();
    if (!match(Token::Type::IDENTIFIER))
      error("Expected an identifier for the parameter name.");

    if (!match(Token::Type::COLON))
      error("Expected a `:` after the parameter name.");

    auto type = parseType();
    return Parameter(ident.value, type);
  }

  [[nodiscard]] TypeInfo Parser::parseType() {
    // TYPE -> IDENTIFIER
    auto token = current();
    if (!match(Token::Type::IDENTIFIER))
      error("Expected a type identifier.");

    return TypeInfo(TypeInfo::toDataType(token.value), token.value);
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

  [[nodiscard]] NodePtr Parser::parseBlock() {
    // BLOCK -> '{' STMT* '}'
    std::vector<NodePtr> body;
    if (!match(Token::Type::LBRACE))
      error("Expected a `{` to start a block.");

    // Parse until we reach the closing brace.
    while (!match(Token::Type::RBRACE))
      body.push_back(parseStmt());

    return create<BlockNode>(std::move(body));
  }

  [[nodiscard]] NodePtr Parser::parseExpr() {
    // EXPR -> BINARY
    return parseBinary(0);
  }

  [[nodiscard]] NodePtr Parser::parseBinary(int min) {
    // BINARY -> UNARY (BINARY_OP UNARY)*
    auto lhs = parseUnary();
    int currentPrec = getPrecedence(current().type);

    while (currentPrec >= min) {
      if (current().isOneOf(BINARY_OPERATOR_TYPES)) {
        auto op = current();
        index++; // Consume the operator.

        // Parse the right-hand side of the binary expression with higher
        // precedence.
        auto rhs = parseBinary(currentPrec + 1);
        lhs = create<BinaryNode>(std::move(lhs), std::move(rhs), op.value);

        currentPrec = getPrecedence(current().type);
      } else {
        break;
      }
    }

    return lhs;
  }

  [[nodiscard]] NodePtr Parser::parseUnary() {
    // UNARY -> (UNARY_OP UNARY | PRIMARY)
    if (current().isOneOf(UNARY_OPERATOR_TYPES)) {
      auto op = current();
      index++; // Consume the operator.

      auto expr = parseUnary();
      return create<UnaryNode>(std::move(expr), op.value);
    }

    // If it's not a unary operator, parse the primary expression.
    return parsePrimary();
  }

  [[nodiscard]] NodePtr Parser::parsePrimary() {
    // PRIMARY -> LITERAL | IDENTIFIER | '(' EXPR ')'
    auto token = current();

    // Check for literals.
    if (match(Token::Type::STRING)) {
      TypeInfo type(TypeInfo::DataType::STRING);
      return create<LiteralNode>(token.value, type);
    }

    else if (match(Token::Type::NUMBER)) {
      TypeInfo type(TypeInfo::DataType::INTEGER);
      return create<LiteralNode>(token.value, type);
    }

    else if (match(Token::Type::IDENTIFIER)) {
      TypeInfo type(TypeInfo::DataType::UNKNOWN);
      auto ident = create<VariableNode>(token.value);

      // Check if it's a function call.
      if (current().is(Token::Type::LPAREN))
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

  [[nodiscard]] NodePtr Parser::parseCall(NodePtr callee) {
    // CALL -> IDENTIFIER '(' (EXPR (',' EXPR)*)? ')'
    std::vector<NodePtr> args;
    if (!match(Token::Type::LPAREN))
      error("Expected a `(` after the function identifier.");

    // Parse until we reach the closing parenthesis.
    if (!current().is(Token::Type::RPAREN))
      do {
        args.push_back(parseExpr());
        if (current().is(Token::Type::RPAREN))
          break;

        if (!match(Token::Type::COMMA))
          error("Expected a `,` or `)` after the argument.");

        // Check for trailing commas.
        else if (match(Token::Type::COMMA) && peek().is(Token::Type::RPAREN))
          error("Expected an argument after the comma.");
      } while (true);

    if (!match(Token::Type::RPAREN))
      error("Expected a `)` after the argument list.");

    return create<CallNode>(std::move(callee), std::move(args));
  }
} // namespace excerpt
