/**
 * @brief Token definitions.
 * @file defs.h
 */

#ifndef VERTE_FRONTEND_LEXER_DEFS_H
#define VERTE_FRONTEND_LEXER_DEFS_H

/**
 * @namespace verte::defs
 * @brief Token definitions.
 */
namespace verte::defs {
/** @defgroup token_defs Token Definitions
 *  @brief Macros for defining various tokens.
 *
 *  These macros define the different types of tokens used in the lexer,
 *  including symbols, operators, keywords, and other token types.
 *
 *  @{
 */

/** @addtogroup token_defs
 *  @{
 */

/**
 * @name Symbol Tokens
 * @brief Macros for defining symbol tokens.
 *
 * These macros define the tokens for various symbols such as parentheses,
 * braces, brackets, and punctuation marks.
 *
 * @{
 */
#define SYMBOLS                                                                \
  _(LPAREN, "(")    /**< Left parenthesis token. */                            \
  _(RPAREN, ")")    /**< Right parenthesis token. */                           \
  _(LBRACE, "{")    /**< Left brace token. */                                  \
  _(RBRACE, "}")    /**< Right brace token. */                                 \
  _(LBRACKET, "[")  /**< Left bracket token. */                                \
  _(RBRACKET, "]")  /**< Right bracket token. */                               \
  _(COMMA, ",")     /**< Comma token. */                                       \
  _(DOT, ".")       /**< Dot token. */                                         \
  _(COLON, ":")     /**< Colon token. */                                       \
  _(SEMICOLON, ";") /**< Semicolon token. */
/** @} */

/**
 * @name Operator Tokens
 * @brief Macros for defining operator tokens.
 *
 * These macros define the tokens for various operators such as assignment,
 * arithmetic, logical, and relational operators.
 *
 * @{
 */
#define OPERATORS                                                              \
  _(ASSIGN, "=")     /**< Assignment operator token. */                        \
  _(BANG, "!")       /**< Logical NOT operator token. */                       \
  _(MINUS, "-")      /**< Minus operator token. */                             \
  _(PLUS, "+")       /**< Plus operator token. */                              \
  _(STAR, "*")       /**< Multiplication operator token. */                    \
  _(SLASH, "/")      /**< Division operator token. */                          \
  _(MOD, "%")        /**< Modulus operator token. */                           \
  _(LESS, "<")       /**< Less than operator token. */                         \
  _(GREATER, ">")    /**< Greater than operator token. */                      \
  _(LT_EQUAL, "<=")  /**< Less than or equal to operator token. */             \
  _(GT_EQUAL, ">=")  /**< Greater than or equal to operator token. */          \
  _(EQUAL, "==")     /**< Equal to operator token. */                          \
  _(NEQ_EQUAL, "!=") /**< Not equal to operator token. */
/** @} */

/**
 * @name Keyword Tokens
 * @brief Macros for defining keyword tokens.
 *
 * These macros define the tokens for various keywords in the language,
 * such as control flow statements, function definitions, and boolean literals.
 *
 * @{
 */
#define KEYWORDS                                                               \
  _(IF, "if")         /**< 'if' keyword token. */                              \
  _(THEN, "then")     /**< 'then' keyword token. */                            \
  _(ELSE, "else")     /**< 'else' keyword token. */                            \
  _(OR, "or")         /**< 'or' keyword token. */                              \
  _(AND, "and")       /**< 'and' keyword token. */                             \
  _(TRUE, "true")     /**< 'true' keyword token. */                            \
  _(FALSE, "false")   /**< 'false' keyword token. */                           \
  _(CONST, "const")   /**< 'const' keyword token. */                           \
  _(FOR, "for")       /**< 'for' keyword token. */                             \
  _(WHILE, "while")   /**< 'while' keyword token. */                           \
  _(FN, "fn")         /**< 'fn' keyword token. */                              \
  _(RETURN, "return") /**< 'return' keyword token. */
/** @} */

/**
 * @name Type Tokens
 * @brief Macros for defining type tokens.
 *
 * These macros define the tokens for various types, such as identifiers,
 * string literals, number literals, and special tokens like invalid and
 * end-of-stream.
 *
 * @{
 */
#define TYPES                                                                  \
  _(IDENTIFIER, "IDENTIFIER") /**< Identifier token. */                        \
  _(STRING, "STRING")         /**< String literal token. */                    \
  _(NUMBER, "NUMBER")         /**< Number literal token. */                    \
  _(INVALID, "INVALID")       /**< Invalid token. */                           \
  _(EOS, "\0")                /**< End of stream token. */
/** @} */

/**
 * @name All Tokens
 * @brief Macro combining all token definitions.
 *
 * This macro is a combination of all the token definition macros,
 * allowing for easier inclusion and processing of all token types.
 *
 * @{
 */
#define TOKENS                                                                 \
  SYMBOLS                                                                      \
  OPERATORS                                                                    \
  KEYWORDS                                                                     \
  TYPES
  /** @} */
  /** @} */ // end of token_defs group

} // namespace verte::defs

#endif // VERTE_FRONTEND_LEXER_DEFS_H
