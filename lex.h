/**
 * 定义了token的类型，以及DFA的状态。
 */

#ifndef LEX_H
#define LEX_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 定义token的类型

/*********************************************************************************
 * token TYPE                  |introduction               | token的种别码
 * TYPE_KEYWORD                | 关键字                     | 1
 * TYPE_IDENTIFIER             | 标识符                     | 2
 * TYPE_NUMBER                 | 整数                       | 3
 * TYPE_FLOAT                  | 浮点数                     | 4
 * TYPE_NOTE                   | 注释                       | 5
 * TYPE_OPERATOR               | 运算符                     | 6
 * TYPE_SEPARATOR              | 分隔符                     | 7
 * TYPE_STRING                 | 字符串                     | 8
 * TYPE_CHAR                   | 字符                       | 9
 * TYPE_ERROR                  | 错误                       | 10
 * TYPE_EOF                    | 文件结束                   | 11
 *
 */

#define TYPE_KEYWORD 1
#define TYPE_IDENTIFIER 2
#define TYPE_NUMBER 3
#define TYPE_FLOAT 4
#define TYPE_NOTE 5
#define TYPE_OPERATOR 6
#define TYPE_SEPARATOR 7
#define TYPE_STRING 8
#define TYPE_CHAR 9
#define TYPE_ERROR 10
#define TYPE_EOF 11

// 定义类型数组
const char *type[] = {"关键字", "标识符", "整数", "浮点数", "注释", "运算符", "分隔符", "字符串", "字符", "错误", "文件结束"};

// 定义DFA的状态

/*********************************************************************************
 * DFA STATE                   |introduction               | token的种别码
 * STATE_START                 | 开始状态                   | 0
 * STATE_KEY_ID                | 关键字或标识符             | 1                     这里的关键字和标识符都是用一个状态来表示的
 * STATE_NUMBER                | 整数                       | 2
 * STATE_FLOAT                 | 浮点数                     | 3
 * STATE_NOTE_ONE              | 单行注释                   | 4
 * STATE_NOTE_MUL              | 多行注释                   | 5
 * STATE_OPERATOR              | 运算符                     | 6
 * STATE_STRING_START          | 字符串                     | 7
 * STATE_CHAR_START            | 字符                       | 8
 * STATE_ERROR                 | 错误                       | 9
 * STATE_END                   | 结束                       | 10
 */

#define STATE_START 0
#define STATE_KEY_ID 1
#define STATE_NUMBER 2
#define STATE_FLOAT 3
#define STATE_NOTE_ONE 4
#define STATE_NOTE_MUL 5
#define STATE_OPERATOR 6
#define STATE_STRING_START 7
#define STATE_CHAR_START 8
#define STATE_ERROR 9
#define STATE_END 10

// 其余的定义
#define OPERATOR_NUM 32
#define SEPARATOR_NUM 8
#define KEYWORD_NUM 32
#define MAX_BUFFER_SIZE 255


// 定义操作符数组

const char *operators[] = {
    "+", "-", "*", "/", "%", "++", "--", "==", "!=", ">", "<", ">=", "<=", "&&", "||", "!", "&", "|", "^", "~", "<<", ">>", "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "."};

// 定义分隔符数组
const char *separators[] = {",", ";", "(", ")", "[", "]", "{", "}"};



// 定义关键字数组,32个
const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
    "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return",
    "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned",
    "void", "volatile", "while"};

// 定义token结构体
typedef struct
{
    int type;    // token的类型
    char *value; // token的值
    int line;    // token所在的行
    int column;  // token所在的列
} Token;

// 定义复用的函数
int isOperator(char c)
{
    for (int i = 0; i < OPERATOR_NUM; i++)
    {
        if (c == operators[i][0])
        {
            return 1;
        }
    }
    return 0;
}

int isOperator2(char *str)
{
    for (int i = 0; i < OPERATOR_NUM; i++)
    {
        if (strcmp(str, operators[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}


int isSeparator(char c)
{
    for (int i = 0; i < SEPARATOR_NUM; i++)
    {
        if (c == separators[i][0])
        {
            return 1;
        }
    }
    return 0;
}
int isKeyword(char *str)
{
    for (int i = 0; i < KEYWORD_NUM; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}


int isDigit(char c)
{
    return c >= '0' && c <= '9';
}
int isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
int isSpace(char c)
{
    return c == ' ' || c == '\t';
}
int isNewLine(char c)
{
    return c == '\n' || c == '\r';
}
int isEscape(char c)
{
    return c == '\\';
}

int isInValid(char c)
{
    return isSpace(c)|| isNewLine(c) || isOperator(c)||isSeparator(c);
}

void printToken(Token token,FILE *fp)
{
    //根据token的类型输出不同的信息
    fprintf(fp, "\t(Value,Type)=(%s,%s)\t(row,col)=(%d,%d)\n",token.value, type[token.type - 1],  token.line, token.column);

}

#endif // LEX_H