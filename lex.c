/**
 * 本文件实现了词法分析器，采用DFA实现。
 *
 *
 */

#include "lex.h"

// 词法分析函数
void LexAnalyse(FILE *fin, FILE *fout)
{
    char cur;                     // 当前字符
    char buffer[MAX_BUFFER_SIZE]; // 单词缓冲区，存放已经读取的字符串
    int bufferIndex;              // 缓冲区指针
    int line = 1, column = 0;     // 行号和列号
    Token curToken;               // 当前token
    curToken.type = 0;
    int state; // 状态
    int type;  // token类型

    int limit_one_line_flag = 0; // 一行的限制标志 ,限制//注释
    int limit_mul_line_flag = 0; // 多行的限制标志 ,限制/*注释

    while (!feof(fin))
    {
        bufferIndex = 0;     // 初始化缓冲区指针
        state = STATE_START; // 初始状态

        // 当状态不是结束状态时，一直循环
        while (state != STATE_END)
        {
            cur = fgetc(fin); // 读取一个字符，则列号加一，然后根据START状态进行确定最开始列号
            column++;
            switch (state)
            {

            case STATE_START:
                if (cur == EOF)
                {
                    state = STATE_END;
                    type = TYPE_EOF;
                    curToken.line = line;
                    curToken.column = column;
                    break;
                }
                else if (isSpace(cur))
                {
                    // 跳过空格
                    state = STATE_START;
                }
                else if (isNewLine(cur))
                {
                    // 换行符
                    line++;     // 行号加一
                    column = 0; // 列号置为0
                    state = STATE_START;
                }
                else if (isLetter(cur) || cur == '_')
                {
                    // 关键字或者标识符
                    state = STATE_KEY_ID;
                    type = TYPE_IDENTIFIER;
                    buffer[bufferIndex++] = cur;
                }

                else if (isDigit(cur))
                {
                    // 数字
                    state = STATE_NUMBER;
                    type = TYPE_NUMBER;
                    buffer[bufferIndex++] = cur;
                }
                else if (cur == '/')
                {
                    // 注释
                    buffer[bufferIndex++] = cur; // 存储当前字符
                    cur = fgetc(fin);            // 读取下一个字符
                    if (cur == '/')
                    {
                        // 单行注释 输出: //+注释内容
                        state = STATE_NOTE_ONE;
                        type = TYPE_NOTE;
                        buffer[bufferIndex++] = cur; // 此时buffer[0] = /,buffer[1] = /
                    }
                    else if (cur == '*')
                    {
                
                        state = STATE_NOTE_MUL;
                        type = TYPE_NOTE;
                        buffer[bufferIndex++] = cur; // 此时buffer[0] = /,buffer[1] = *
                    }
                    else
                    {
                        // 运算符
                        state = STATE_OPERATOR;
                        type = TYPE_OPERATOR;
                        bufferIndex--;
                        fseek(fin, -1, SEEK_CUR); // 回退一个字符
                        column--;
                    }
                }
                else if (isOperator(cur))
                {
                    state = STATE_OPERATOR;
                    type = TYPE_OPERATOR;
                    buffer[bufferIndex++] = cur;
                }
                else if (isSeparator(cur))
                {
                    // 分隔符 如逗号、分号等
                    state = STATE_END;
                    type = TYPE_SEPARATOR;
                    buffer[bufferIndex++] = cur;
                }
                else if (cur == '\"')
                {
                    // 字符串
                    state = STATE_STRING_START;
                    type = TYPE_STRING;
                    buffer[bufferIndex++] = cur;
                }
                else if (cur == '\'')
                {
                    // 字符
                    state = STATE_CHAR_START;
                    type = TYPE_CHAR;
                    buffer[bufferIndex++] = cur;
                }
                else
                {
                    state = STATE_END;
                    type = TYPE_ERROR;
                }

                // 记录token的行号和列号
                curToken.line = line;
                curToken.column = column;
                break;
            case STATE_KEY_ID:
                if (isLetter(cur) || isDigit(cur) || cur == '_')
                {
                    buffer[bufferIndex++] = cur;
                }
                else if (cur == '.')
                {
                    // 结构体成员访问符，如a.b
                    buffer[bufferIndex++] = cur;
                    cur = fgetc(fin); // 读取下一个字符
                    if (isLetter(cur) || cur == '_')
                    {
                        buffer[bufferIndex++] = cur;
                    }
                    else if (isInValid(cur))
                    {
                        // 读取到结束符，如空格、换行符、逗号等
                        fseek(fin, -1, SEEK_CUR); // 回退一个字符
                        state = STATE_END;
                        type = TYPE_ERROR;
                        column--;
                    }
                    else
                    {
                        buffer[bufferIndex++] = cur;
                        type = TYPE_ERROR;
                    }
                }
                else if (isInValid(cur))
                {
                    state = STATE_END;
                    fseek(fin, -1, SEEK_CUR); // 回退一个字符
                    column--;
                }
                else
                {
                    state = STATE_END;
                    type = TYPE_ERROR;
                }
                break;
            case STATE_NUMBER: //  数字状态 只能从开始状态进入 123 而a123不是数字,是标识符
                if (isDigit(cur))
                {
                    // 数字后面还是数字
                    buffer[bufferIndex++] = cur;
                }
                else if (cur == '.')
                {
                    // 浮点数 进入浮点数状态，记得保留小数点前的数字

                    buffer[bufferIndex++] = cur; // 存储小数点
                    state = STATE_FLOAT;
                    type = TYPE_FLOAT;
                }
                else if (isInValid(cur))
                {
                    // 数字后面是结束符或者操作符
                    buffer[bufferIndex] = '\0';
                    fseek(fin, -1, SEEK_CUR); // 回退一个字符
                    column--;
                    state = STATE_END;
                }
                else
                {
                    buffer[bufferIndex++] = cur;
                    type = TYPE_ERROR;
                }
                break;
            case STATE_FLOAT: // 浮点数状态，只能从数字状态进入
                // 浮点数，如1.2 buffer数组里面已经存放了1. 现在需要读取小数点后面的数字
                if (isDigit(cur))
                {
                    buffer[bufferIndex++] = cur;
                }
                else if (isInValid(cur))
                {
                    // 小数点后面是结束符或者操作符
                    buffer[bufferIndex] = '\0';
                    fseek(fin, -1, SEEK_CUR); // 回退一个字符
                    column--;
                    state = STATE_END;
                }
                else
                {
                    buffer[bufferIndex++] = cur;
                    type = TYPE_ERROR;
                }
                break;
            case STATE_NOTE_ONE: // 单行注释 token存放的是 //+注释内容
                while (cur != '\n' && cur != EOF)
                {
                    buffer[bufferIndex++] = cur;
                    cur = fgetc(fin);
                }
                buffer[bufferIndex] = '\0';
                fseek(fin, -1, SEEK_CUR); // 回退一个字符
                column--;
                state = STATE_END;
                break;
            case STATE_NOTE_MUL: //  多行注释需要处理，token一般不存放注释内容，因为注释内容不需要分析，而且注释内容通常多行，需要确定*/的位置
                while (cur != EOF)
                {
                    if (cur == '*')
                    {
                        cur = fgetc(fin);
                        if (cur == '/')
                        {
                            //读到*/结束，后面还有内容
                            buffer[bufferIndex++] = '*';
                            buffer[bufferIndex++] = '/';
                            buffer[bufferIndex] = '\0';
                            state = STATE_END;
                            break;
                        }
                        else if(isNewLine(cur))
                        {
                            line++;
                            column = 0;
                            buffer[bufferIndex++] = '*';
                            buffer[bufferIndex++] = cur;
                        }
                        else
                        {
                            buffer[bufferIndex++] = '*';
                            buffer[bufferIndex++] = cur;
                        }
                    }
                    else if (isNewLine(cur))
                    {
                        line++;
                        column = 0;
                        buffer[bufferIndex++] = cur;
                    }
                    else
                    {
                        buffer[bufferIndex++] = cur;
                    }
                    cur = fgetc(fin);
                    column++;
                }
                if (cur == EOF)
                {
                    state = STATE_END;
                    type = TYPE_ERROR;
                }
                break;
            case STATE_OPERATOR: // 运算符状态
                // 分析运算符。如果是单个运算符，如+、-、*、/等，直接输出。如果是双字符运算符，如>=、<=、==等，需要判断下一个字符
                if (isOperator(cur))
                {
                    // 双字符运算符,判断是否该双字符运算符合法
                    // 将两个字符与运算符表中的双字符运算符进行比较，三字符运算符不考虑
                    // 判断是否是双字符运算符
                    char temp[3] = {buffer[0], cur, '\0'};
                    if (isOperator2(temp))
                    {
                        // 双字符运算符
                        buffer[bufferIndex++] = cur; // 存储第二个字符
                        buffer[bufferIndex] = '\0';
                        state = STATE_END;
                    }
                    else
                    {
                        // 不合法的双字符运算符，如+*、-+等，将第一个字符输出，回退一个字符
                        buffer[bufferIndex] = '\0'; // 存储第一个字符
                        fseek(fin, -1, SEEK_CUR);   // 回退一个字符
                        column--;
                        state = STATE_END;
                        type = TYPE_OPERATOR;
                    }
                }
                else
                {
                    // 单字符运算符
                    fseek(fin, -1, SEEK_CUR); // 回退一个字符
                    column--;
                    state = STATE_END;
                    type = TYPE_OPERATOR;
                }
                break;
            case STATE_STRING_START:              // 字符串开始状态
                while (cur != '\"' && cur != EOF) // 读取字符串内容
                {
                    buffer[bufferIndex++] = cur;
                    cur = fgetc(fin);
                }
                if (cur == '\"')
                {
                    // 字符串正常结束
                    buffer[bufferIndex++] = cur;
                    buffer[bufferIndex] = '\0';
                    state = STATE_END;
                }
                // 读不到"，字符串不正常结束
                else if (isNewLine(cur))
                {
                    line++;     // 行号加一
                    column = 1; // 列号置为1
                    buffer[bufferIndex++] = cur;
                    // 对于字符串，换行符不做处理，留到语法分析阶段处理
                }
                else if (cur == EOF)
                {
                    state = STATE_END;
                    type = TYPE_ERROR;
                }
                break;

            case STATE_CHAR_START: // 字符开始状态
                // 字符只能是一个字符，所以只需要读取一个字符
                while (cur != '\'' && cur != EOF) // 读取字符内容
                {
                    buffer[bufferIndex++] = cur;
                    cur = fgetc(fin);
                }
                if (cur == '\'')
                {
                    // 字符正常结束
                    buffer[bufferIndex++] = cur;
                    buffer[bufferIndex] = '\0';
                    state = STATE_END;
                }
                // 读不到'，字符不正常结束
                else if (isNewLine(cur))
                {
                    line++;     // 行号加一
                    column = 1; // 列号置为1
                    buffer[bufferIndex++] = cur;
                    // 对于字符，换行符不做处理，留到语法分析阶段处理
                }
                else if (cur == EOF)
                {
                    state = STATE_END;
                    type = TYPE_ERROR;
                }
                break;
            case STATE_ERROR:
                // 错误状态
                type = TYPE_ERROR;
                state = STATE_END;
                break;
            case STATE_END:
                break;
            default:
                break;
            }
        }

        // 结束状态
        buffer[bufferIndex] = '\0'; // 字符串结束符
        if (type == TYPE_EOF)
        {
            curToken.value = "EOF";
        }
        else
        {
            curToken.value = buffer;
        }

        curToken.type = type;
        printToken(curToken, fout);
    }

    printf("词法分析结束\n");
}

int main()
{
    system("chcp 65001");
    FILE *fin = fopen("test.c", "r");
    FILE *fout = fopen("token.txt", "w");
    LexAnalyse(fin, fout);
    fclose(fin);
    fclose(fout);
    return 0;
}
