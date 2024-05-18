/**
 * @brief Parser implementation.
 * @file parser.cpp
 */

#include "excerpt/parser/parser.hpp"

namespace excerpt {
  std::unique_ptr<ProgramAST> Parser::parse() {
    return std::make_unique<ProgramAST>(std::move(parseProgram()));
  }

  std::vector<ExprPtr> Parser::parseProgram() {
    std::vector<ExprPtr> program;

    // Parse all the expressions in the program.
    while (!current.is(Token::Type::END)) {
      program.push_back(parseExpr());
    }

    return program;
  }

  ExprPtr Parser::parseExpr() {
    ExprPtr expr = parseAtom();

    // If the current token is an operator, we parse the right-hand side.
    if (current.isOneOf(OPERATOR_TYPES)) {
      int precedence = getPrecedence(current.type);
      if (precedence > PREC_NONE) {
        return parseBinaryExpr(std::move(expr), precedence);
      }
    }

    return expr;
  }

  ExprPtr Parser::parseVarDecl() {
    std::string name = current.value;

    advance(); // Consume the identifier
    if (current.type != Token::Type::COLON) {
      error("Expected ':' after variable name");
    }

    advance(); // Consume ':'
    if (current.type != Token::Type::IDENTIFIER) {
      error("Expected data type after ':'");
    }

    // The data type of the variable.
    std::string data_type = current.value;

    advance(); // Consume '='
    if (current.type != Token::Type::EQUAL) {
      error("Expected '=' after variable type");
    }

    advance();
    ExprPtr value = parseExpr();

    expect(Token::Type::SEMICOLON, "Expected `;` after variable declaration");
    return create<VarDeclAST>(name, data_type, std::move(value));
  }

  ExprPtr Parser::parseBinaryExpr(ExprPtr lhs, int min) {
    while (true) {
      int precedence = getPrecedence(current.type);

      if (precedence < min)
        return lhs;

      // If the current token is an operator, we parse the right-hand side.
      if (precedence == PREC_NONE)
        return lhs;

      std::string op = current.value;
      advance();

      ExprPtr rhs = parseAtom();
      while (getPrecedence(current.type) > precedence)
        // Parse the right-hand side expression with higher precedence.
        rhs = parseBinaryExpr(std::move(rhs), precedence + 1);

      lhs = create<BinaryAST>(op, std::move(lhs), std::move(rhs));
    }
  }

  ExprPtr Parser::parseAtom() {
    ExprPtr result = nullptr;

    // Parse the atom based on the current token.
    switch (current.type) {
      case Token::Type::IDENTIFIER: {
        if (peek().is(Token::Type::COLON)) {
          return parseVarDecl();
        }

        else {
          result = create<IdentAST>(current.value);
          advance();
          break;
        }
      }

      case Token::Type::NUMBER: {
        result = create<NumberAST>(std::stod(current.value));
        advance();
        break;
      }

      case Token::Type::STRING: {
        result = create<StringAST>(current.value);
        advance();
        break;
      }

      case Token::Type::LPAREN: {
        advance(); // Consume '('
        ExprPtr expr = parseExpr();

        if (current.type != Token::Type::RPAREN) {
          error("Expected ')' after expression");
        }

        advance(); // Consume ')'
        return expr;
      }

      default:
        break;
    }

    // TODO: Change the way we handle chained operators.
    if (current.isOneOf(OPERATOR_TYPES) && result != nullptr) {
      return parseBinaryExpr(std::move(result), getPrecedence(current.type));
    }

    if (result == nullptr) {
      error("Unexpected token: " + current.toString());
    }

    return result;
  }
} // namespace excerpt
