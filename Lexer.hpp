#pragma once

#include <iostream>
#include <string>
#include <map>

#include "Token.hpp"

class Lexer
{
public:
    Lexer() = default;
    ~Lexer() = default;

    /**
     * gets next token that is on standard input
     * */
    int Gettok();

    static std::string tokToString(Token tok);

    /**
     * returns string representation of identifier
     *
     * note: if no new indentifier has been handled since the last one, this value stays the same
     * */
    const std::string &IdentifierStr() const
    {
        return this->m_IdentifierStr;
    }

    /**
     * returns numerical representation of number
     *
     * note: if no new number has been handled since the last one, this value stays the same
     * */
    int NumVal() const
    {
        return this->m_NumVal;
    }

private:
    std::string m_IdentifierStr;
    int m_NumVal;
    char m_LastChar = ' ';

    /**
     * returns corresponding token that is made out of characters only
     * */
    int GetWordTok();

    /**
     * handles numbers and returns number token, can handle base 10, 8, 16
     * */
    int GetNumberTok();

    /**
     * handles whole line of comment
     * */
    void CleanComments();

    /**
     * handles token that are made out of special characters
     * */
    int OtherTok();

    /**
     * handles incorrect or other non recognised symbols
     * */
    int ErrorLexer();
};

/*
 * Lexer returns tokens [0-255] if it is an unknown character,
 * otherwise one of these for known things.
 * Here are all valid tokens:
 */
