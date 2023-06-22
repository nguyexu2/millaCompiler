#pragma once 

#include <map>
#include <string>

enum Token
{
    tok_eof = -100000, //declaring as a negative number forces all the tokens below to be negative too

    // numbers and identifiers
    tok_identifier,
    tok_number,

    // keywords
    tok_program,
    tok_function,
    tok_forward,
    tok_begin,
    tok_end,
    tok_const,
    tok_procedure,

    tok_exit,
    tok_var,
    tok_integer,
    tok_array,
    tok_of,

    tok_if,
    tok_then,
    tok_else,

    tok_while,
    tok_for,
    tok_do,
    tok_to,
    tok_downto,
    tok_break,

    // 1-character operators
    tok_equal,
    tok_less,
    tok_greater,

    tok_plus,
    tok_minus,
    tok_multiply,
    //tok_div is a 3char operator

    tok_semicolon,
    tok_colon,
    tok_dot,
    tok_comma,

    tok_leftBracket,
    tok_rightBracket,
    tok_leftArrBracket,
    tok_rightArrBracket,

    // 2-character operators
    tok_notEqual,
    tok_lessEqual,
    tok_greaterEqual,
    tok_assign,
    tok_or,

    // 3-character operators (keywords)
    tok_mod,
    tok_div,
    tok_not,
    tok_and,
    tok_xor,

    tok_error
};

const std::map<std::string, int> g_LexerTable{
    {"program", tok_program},
    {"function", tok_function},
    {"forward", tok_forward},
    {"begin", tok_begin},
    {"end", tok_end},
    {"const", tok_const},
    {"procedure", tok_procedure},

    {"exit", tok_exit},
    {"var", tok_var},
    {"integer", tok_integer},
    {"array", tok_array},
    {"of", tok_of},

    {"if", tok_if},
    {"then", tok_then},
    {"else", tok_else},
    
    {"while", tok_while},
    {"for", tok_for},
    {"do", tok_do},
    {"to", tok_to},
    {"downto", tok_downto},
    {"break", tok_break},

    {"=", tok_equal},
    {"<", tok_less},
    {">", tok_greater},

    {"+", tok_plus},
    {"-", tok_minus},
    {"*", tok_multiply},

    {";", tok_semicolon},
    {":", tok_colon},
    {".", tok_dot},
    {",", tok_comma},

    {"(", tok_leftBracket},
    {")", tok_rightBracket},
    {"[", tok_leftArrBracket},
    {"]", tok_rightArrBracket},

    {"<>", tok_notEqual},
    {"<=", tok_lessEqual},
    {">=", tok_greaterEqual},
    {":=", tok_assign},
    {"or", tok_or},

    {"mod", tok_mod},
    {"div", tok_div},
    {"not", tok_not},
    {"and", tok_and},
    {"xor", tok_xor}};
