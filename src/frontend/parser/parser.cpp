/**
 * @brief Parser implementation.
 * @file parser.cpp
 */

#include "verte/frontend/parser/parser.hpp"
#include "verte/errors.hpp"
#include <memory>

namespace verte::nodes {
  /**
   * @brief Clone an expression node.
   * @param expr The expression to clone.
   * @return The cloned expression.
   */
  NodePtr cloneExpr(const NodePtr &expr) {
    // Handle null case.
    if (!expr) {
      return nullptr;
    }

    // Variable node.
    if (const auto *var = dynamic_cast<const VariableNode *>(expr.get())) {
      return std::make_unique<VariableNode>(var->getName());
    }

    // Literal node.
    if (const auto *lit = dynamic_cast<const LiteralNode *>(expr.get())) {
      return std::make_unique<LiteralNode>(lit->getValue(), lit->getType());
    }

    // Binary node (recursive).
    if (const auto *bin = dynamic_cast<const BinaryNode *>(expr.get())) {
      return std::make_unique<BinaryNode>(
          cloneExpr(bin->getLHS()), cloneExpr(bin->getRHS()), bin->getOp());
    }

    // Unary node (recursive).
    if (const auto *unary = dynamic_cast<const UnaryNode *>(expr.get())) {
      return std::make_unique<UnaryNode>(cloneExpr(unary->getOperand()),
                                         unary->getOp());
    }

    // Call node (recursive for arguments).
    if (const auto *call = dynamic_cast<const CallNode *>(expr.get())) {
      std::vector<NodePtr> clonedArgs;
      clonedArgs.reserve(call->getArgs().size()); // Pre-allocate.

      for (const auto &arg : call->getArgs()) {
        clonedArgs.push_back(cloneExpr(arg));
      }

      return std::make_unique<CallNode>(
          std::make_unique<VariableNode>(call->getCallee()->getName()),
          std::move(clonedArgs));
    }

    // Unsupported node type.
    throw std::runtime_error("Cannot clone expression: unsupported node type.");
  }
} // namespace verte::nodes

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
    if ((token.is(Token::Type::IDENTIFIER) || token.is(Token::Type::LET)) &&
        (next.is(Token::Type::IDENTIFIER) || next.is(Token::Type::COLON)))
      return parseVarDecl();

    // Check if the current token is a variable assignment.
    else if (token.is(Token::Type::IDENTIFIER) && next.is(Token::Type::ASSIGN))
      return parseAssign();

    // Check if the current token is a match statement.
    else if (token.is(Token::Type::MATCH))
      return parseMatch();

    // Check if the current token is a function declaration.
    else if (token.is(Token::Type::FUN))
      return parseFuncDecl();

    // Check if the current token is a return statement.
    else if (token.is(Token::Type::RETURN))
      return parseReturn();

    // Default to an expression statement.
    return parseExprStmt();
  }

  [[nodiscard]] NodePtr Parser::parseVarDecl() {
    // VAR_DECL -> (let)? IDENTIFIER ':' '@' TYPE '=' EXPR ';'
    bool isConst = false;
    if (match(Token::Type::LET))
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

    if (!match(Token::Type::ASSIGN))
      error("Expected an `=` after the identifier.");

    auto expr = parseExpr();
    if (!match(Token::Type::SEMICOLON))
      error("Expected a `;` after the expression.");

    return create<AssignNode>(ident.getValue(), std::move(expr));
  }

  [[nodiscard]] NodePtr Parser::parseMatch() {
    // MATCH_STMT -> match (EXPR)? where GUARD+ (else GUARD_BODY)? end
    if (!match(Token::Type::MATCH))
      error("Expected `match` keyword.");

    // Check for optional expression.
    NodePtr matchExpr = nullptr;
    if (!currentToken().is(Token::Type::WHERE)) {
      matchExpr = parseExpr();
    }

    if (!match(Token::Type::WHERE))
      error("Expected `where` after match expression.");

    // Parse guards.
    std::vector<IfNodePtr> guards;
    while (currentToken().is(Token::Type::WHEN)) {
      guards.push_back(parseGuard(matchExpr));
    }

    if (guards.empty())
      error("Match statement must have at least one guard.");

    // Parse optional default case.
    BlockPtr defaultBody = nullptr;
    if (match(Token::Type::ELSE)) {
      defaultBody = parseGuardBody();
    }

    if (!match(Token::Type::END))
      error("Expected `end` to close match statement.");

    // Build nested if-else structure.
    return buildNestedMatch(std::move(guards), std::move(defaultBody));
  }

  [[nodiscard]] IfNodePtr Parser::parseGuard(const NodePtr &matchExpr) {
    // GUARD -> when GUARD_EXPR GUARD_BODY
    if (!match(Token::Type::WHEN))
      error("Expected `when` keyword.");

    NodePtr condition = parseGuardExpr(matchExpr);
    BlockPtr body = parseGuardBody();

    return std::make_unique<IfNode>(std::move(condition), std::move(body));
  }

  [[nodiscard]] NodePtr Parser::parseGuardExpr(const NodePtr &matchExpr) {
    // GUARD_EXPR -> EXPR (for boolean guards)
    //            -> COMPARISON_OP EXPR (for comparison matching)
    //            -> EXPR (for value matching, becomes ==)
    if (matchExpr == nullptr) {
      return parseExpr();
    }

    // Check if it's a comparison operator.
    auto token = currentToken();
    if (token.isOneOf({Token::Type::GREATER, Token::Type::GT_EQUAL,
                       Token::Type::LESS, Token::Type::LT_EQUAL,
                       Token::Type::EQUAL, Token::Type::NEQ_EQUAL})) {

      std::string op = token.getValue();
      (void)nextToken(); // Consume operator.

      NodePtr value = parseExpr();
      return create<BinaryNode>(cloneExpr(matchExpr), std::move(value), op);
    }

    NodePtr value = parseExpr();
    return create<BinaryNode>(cloneExpr(matchExpr), std::move(value), "==");
  }

  [[nodiscard]] BlockPtr Parser::parseGuardBody() {
    // GUARD_BODY -> '->' STMT
    //            -> do STMT* end
    if (match(Token::Type::ARROW)) {
      NodePtr stmt = parseStmt();

      std::vector<NodePtr> body;
      body.push_back(std::move(stmt));

      return std::make_unique<BlockNode>(std::move(body));
    }

    else if (currentToken().is(Token::Type::DO)) {
      return parseBlock();
    }

    else {
      error("Expected `->` or `do` for guard body.");
    }
  }

  [[nodiscard]] NodePtr Parser::parseFuncDecl() {
    // FUNC_DECL -> fun IDENTIFIER '(' PARAMS ')' ':' '@' TYPE (';' | do STMT*
    // end)
    if (!match(Token::Type::FUN))
      error("Expected a `fun` for the function declaration.");

    auto proto = parseProto();

    if (match(Token::Type::SEMICOLON))
      return proto;

    else if (currentToken().is(Token::Type::DO))
      return create<FuncDeclNode>(std::move(proto), parseBlock());

    error("Expected a `;` or `do` after the function prototype.");
  }

  [[nodiscard]] NodePtr Parser::buildNestedMatch(std::vector<IfNodePtr> guards,
                                                 BlockPtr defaultBody) {
    if (guards.empty()) {
      return defaultBody;
    }

    // Take the first guard.
    IfNodePtr first = std::move(guards[0]);
    guards.erase(guards.begin());

    // If no more guards, check for default.
    if (guards.empty()) {
      if (defaultBody) {
        return create<IfElseNode>(std::move(first), std::move(defaultBody));
      } else {
        return first;
      }
    }

    // Recursively build the rest.
    NodePtr rest = buildNestedMatch(std::move(guards), std::move(defaultBody));

    // Wrap rest in a BlockNode for the else branch.
    std::vector<NodePtr> elseBody;
    elseBody.push_back(std::move(rest));

    BlockPtr elseBlock = std::make_unique<BlockNode>(std::move(elseBody));
    return create<IfElseNode>(std::move(first), std::move(elseBlock));
  }

  [[nodiscard]] ProtoPtr Parser::parseProto() {
    // PROTO -> IDENTIFIER '(' PARAMS ')' ':' '@' TYPE
    auto ident = currentToken();
    if (!match(Token::Type::IDENTIFIER))
      error("Expected an identifier for the function name.");

    if (!match(Token::Type::LPAREN))
      error("Expected a `(` after the function name.");

    std::vector<Parameter> params = parseParams();

    if (!match(Token::Type::COLON))
      error("Expected `:` before return type.");

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
    // TYPE -> '@' IDENTIFIER
    if (!match(Token::Type::AT))
      error("Expected '@' before type identifier.");

    auto token = currentToken();
    if (!match(Token::Type::IDENTIFIER))
      error("Expected a type identifier after '@'.");

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
    // BLOCK -> do STMT* end
    std::vector<NodePtr> body;

    if (!match(Token::Type::DO))
      error("Expected a `do` to start a block.");

    // Parse until we reach the closing 'end'.
    while (!match(Token::Type::END))
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
