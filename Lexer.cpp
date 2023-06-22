#include "Lexer.hpp"

std::string Lexer::tokToString(Token tok)
{    
    if(tok == tok_identifier)
        return "ident";
    else if(tok == tok_number)
        return "number";

    for(const auto &x : g_LexerTable)
    {
        if(x.second == tok)
            return x.first;
    }

    return std::string(1, tok);
}


/*
 * Function to return the next token from standard input
 * the variable 'm_IdentifierStr' is set there in case of an identifier,
 * the variable 'm_NumVal' is set there in case of a number.
 */

int Lexer::Gettok()
{
    while (isspace(m_LastChar))
        m_LastChar = getchar();
    
    if (isalpha(m_LastChar))
    {
        return GetWordTok();
    }
    else if (isdigit(m_LastChar) || m_LastChar == '$' || m_LastChar == '&')
    {
        return GetNumberTok();
    }
    else if (m_LastChar == '#')
    {
        CleanComments();
        return Gettok();
    }
    else if (m_LastChar == EOF)
        return tok_eof;

    return OtherTok();
}

int Lexer::GetWordTok()
{
    m_IdentifierStr = m_LastChar;

    while (isalnum(m_LastChar = getchar()) || m_LastChar == '_')
        m_IdentifierStr += m_LastChar;

    auto tableFind = g_LexerTable.find(m_IdentifierStr);
    if (tableFind != g_LexerTable.end())
        return tableFind->second;

    return tok_identifier;
}

int Lexer::GetNumberTok()
{
    int base = 10;
    std::string numStr;

    if(m_LastChar == '$')
    {
        base = 16;
        m_LastChar = getchar();
    
        do
        {
            numStr += m_LastChar;
            m_LastChar = getchar();
        } while ( isdigit(m_LastChar) 
            || (isalpha(m_LastChar) && (m_LastChar - 'a' <  16-10) ) );

    }
    else if(m_LastChar == '&')
    {
        base = 8;
        m_LastChar = getchar();

        do
        {
            numStr += m_LastChar;
            m_LastChar = getchar();
        } while (isdigit(m_LastChar)  &&  (m_LastChar - '0' <  8 - 0 ));

    }
    else
    {
        do
        {
            numStr += m_LastChar;
            m_LastChar = getchar();
        } while (isdigit(m_LastChar));
    }

    m_NumVal = strtol(numStr.c_str(), nullptr, base);
    return tok_number;
}

void Lexer::CleanComments()
{
    do
        m_LastChar = getchar();
    while (m_LastChar != EOF && m_LastChar != '\n' && m_LastChar != '\r');

    /*
    std::string line;
    std::getline(std::cin, line);
    m_LastChar = getchar();

    */
}

int Lexer::OtherTok()
{
    if (m_LastChar == '<')
    {
        m_LastChar = getchar();
        if (m_LastChar == '=')
        {
            m_LastChar = getchar();
            return tok_lessEqual;
        }
        else if (m_LastChar == '>')
        {
            m_LastChar = getchar();
            return tok_notEqual;
        }
        else
            return tok_less;
    }
    else if (m_LastChar == '>')
    {
        m_LastChar = getchar();
        if (m_LastChar == '=')
        {
            m_LastChar = getchar();
            return tok_greaterEqual;
        }
        else
            return tok_greater;
    }
    else if (m_LastChar == ':')
    {
        m_LastChar = getchar();
        if (m_LastChar == '=')
        {
            m_LastChar = getchar();
            return tok_assign;
        }
        else
            return tok_colon;
    }

    auto lexerTableFind = g_LexerTable.find( std::string{m_LastChar} );
    if(lexerTableFind != g_LexerTable.end())
    {
        m_LastChar = getchar();
        return lexerTableFind->second;
    }

    return ErrorLexer();
}

int Lexer::ErrorLexer()
{
    //std::cerr << "unrecognised token in lexer\n";
    char retErr = m_LastChar;
    m_LastChar = getchar();
    return retErr;
}

