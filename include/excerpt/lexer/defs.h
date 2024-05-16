/**
 * @brief Token definitions.
 * @file defs.h
 */

#ifndef EXCERPT_DEFS_H
#define EXCERPT_DEFS_H

namespace excerpt {
#define SYMBOLS                                                                \
  _(LPAREN, "(")                                                               \
  _(RPAREN, ")")                                                               \
  _(LBRACE, "{")                                                               \
  _(RBRACE, "}")                                                               \
  _(LBRACKET, "[")                                                             \
  _(RBRACKET, "]")                                                             \
  _(COMMA, ",")                                                                \
  _(DOT, ".")                                                                  \
  _(MINUS, "-")                                                                \
  _(PLUS, "+")                                                                 \
  _(SEMICOLON, ";")

#define OPERATORS                                                              \
  _(EQUAL, "=")                                                                \
  _(BANG, "!")                                                                 \
  _(STAR, "*")                                                                 \
  _(SLASH, "/")                                                                \
  _(LESS, "<")                                                                 \
  _(GREATER, ">")                                                              \
  _(EQUAL_EQUAL, "==")                                                         \
  _(BANG_EQUAL, "!=")                                                          \
  _(LESS_EQUAL, "<=")                                                          \
  _(GREATER_EQUAL, ">=")

#define KEYWORDS                                                               \
  _(IF, "if")                                                                  \
  _(ELSE, "else")                                                              \
  _(OR, "or")                                                                  \
  _(TRUE, "true")                                                              \
  _(FALSE, "false")                                                            \
  _(FOR, "for")                                                                \
  _(WHILE, "while")                                                            \
  _(SUPER, "super")                                                            \
  _(THIS, "this")                                                              \
  _(NONE, "none")                                                              \
  _(FN, "fn")

#define TYPES                                                                  \
  _(IDENTIFIER, "IDENTIFIER")                                                  \
  _(STRING, "STRING")                                                          \
  _(NUMBER, "NUMBER")                                                          \
  _(INVALID, "INVALID")                                                        \
  _(END, "\0")

#define TOKENS                                                                 \
  SYMBOLS                                                                      \
  OPERATORS                                                                    \
  KEYWORDS                                                                     \
  TYPES
} // namespace excerpt

#endif // EXCERPT_DEFS_H
