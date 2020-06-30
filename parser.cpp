//parser.cpp
// IDE: CLion
// Created by DariusYoung on 2020/6/23.
//
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
using namespace std;


//过程嵌套的最大层数
#define levmax 3

enum object {CONSTANT,VARIABLE,PROCEDUR};

struct Table
{
    char name[20];
    int VAL;  //在语法分析中并没有用到
    int LEVEL; /* 所处层，仅const不使用 */
    int ADR; //在语法分析中并没有用到
    enum object kind; 

    int SIZE; //在语法分析中并没有用到
};

struct Table table[200];

int block(int lev,int tx);
int factor(int* TX,int lev);
int term(int* TX,int lev);
int condition(int* TX,int lev);
int expression(int* TX,int lev);
int statement(int* TX,int lev);
int vardeclaration(int* TX,int lev,int* DX);
int constdeclaration(int* TX,int lev,int* DX);
int searchTable(char* idt,int tx);
void addTable(enum object KIND,int* TX,int lev,int* DX);
void error(int errorNum);
///////////////////////////////////////
#define BEGINSYM 1
#define ENDSYM 2
#define IFSYM 3
#define THENSYM 4
#define WHILESYM 5
#define DOSYM 6
#define CONSTSYM 7
#define VARSYM 8
#define CALLSYM 9
#define PROCEDURESYM 10
#define ODDSYM 11
#define READSYM 12
#define WRITESYM 13


#define ADDSYM 14
#define SUBSYM 15
#define MULTISYM 16
#define DIVSYM 17


#define EQ 18

//#
#define NE 19

//<
#define LT 20

//>
#define GT 21

//<=
#define LE 22

//>=
#define  GE 23

//:=
#define ASGN 24

//(
#define LPAREN  25

//)
#define RPAREN  26

//,
#define COLON  27

//.
#define PERIOD  0

//;
#define SEMICOLON  28

#define NUMBER 29
#define IDENT   30
#define ERROR -1

#define  symNum 32;
int cx = 0;
int sym = 1;
int Line = 1;
int sumOfError = 0;
int sum = 0;
char token[10];
char *Code;
char ch;
int index = 0;
char * keyword[13] = {"begin", "end", "if", "then", "while", "do", "const", "var", "call", "procedure", "odd", "read", "write"};
int token_index = 0;

bool isNumber(char c);
void NumberProc();
bool isWord(char c);
void IDENTProc();
void keywordProc();
void getCH();
void getSym();
void searchKey();
void markProc();

//判断字符是否为数字
bool isNumber(char c){
    return c >= '0' && c <= '9';

}

//自定义数字处理
void NumberProc(){
    sum = 0;
    while(isNumber(ch)){
        sum = sum * 10 + ch - '0';
        ch = Code[index++];
    }
    index--;
    sym = NUMBER;
}

//判断是否为字符
bool isWord(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

//自定义标识符处理
void IDENTProc(){
    token_index = 0;
    while (isNumber(ch) || isWord(ch))
    {
        token[token_index++] = ch;
        ch = Code[index++];
//        cout<<ch<<endl;
    }
    token[token_index++] = '\0';
    index--;
    sym = IDENT;
}

void markProc(){
    switch (ch){
        case '<':
            token_index = 0;
            token[token_index++] = ch;
            ch = Code[index++];
            if (ch == '=')
            {
                sym = LE;
                token[token_index++] = ch;
            }
            else
            {
                sym = LT;
                index--;
            }
            break;
        case ':':
            token_index = 0;
            token[token_index++] = ch;
            ch = Code[index++];
            if (ch == '=')
            {
                token[token_index++] = ch;
                sym = ASGN;
            }
            else
            {
                sym = ERROR;
            }
            break;
        case '>':
            token_index = 0;
            token[token_index++] = ch;
            ch = Code[index++];
            if (ch == '=')
            {
                sym = GE;
                token[token_index++] = ch;
            }
            else
            {
                sym = GT;
                index--;
            }
            break;

        case '*':
            sym = MULTISYM;
            token[0] = ch;
            break;
        case '/':
            sym = DIVSYM;
            token[0] = ch;
            break;
        case '+':
            sym = ADDSYM;
            token[0] = ch;
            break;
        case '-':
            sym = SUBSYM;
            token[0] = ch;
            break;
        case '=':
            sym = EQ;
            token[0] = ch;
            break;
        case ';':
            sym = SEMICOLON;
            token[0] = ch;
            break;
        case '(':
            sym = LPAREN;
            token[0] = ch;
            break;
        case ')':
            sym = RPAREN;
            token[0] = ch;
            break;
        case ',':
            sym = COLON;
            token[0] = ch;
            break;
        case '#':
            sym = NE;
            token[0] = ch;
            break;
        case '.':
            sym = PERIOD;
            token[0] = ch;
            break;
        default:
            sym = ERROR;
            break;
    }
}


//查找当前标识符是否已定义
void searchKey(){
    for(int i=0; i<13; i++){
        if(strcmp(token, keyword[i]) == 0){
            sym = i + 1;
            break;
        }
    }
}

//保留字处理
void keywordProc(){

}

void getCH(){
    ch = Code[index++];
//    cout<<ch<<endl;
    while(ch == ' ' || ch == '\n'){
        if(ch == '\n'){
            Line++;
        }
        ch = Code[index++];
    }
}

void getSym(){
    memset(token, 0, sizeof(token));
    getCH();
    if(isWord(ch)){
        IDENTProc();
        searchKey();
    }
    else if(isNumber(ch)){
        NumberProc();
    }
    else{
        markProc();
    }
//    cout << sym << endl;
//    cout << sum << " or " << token <<endl;
}

void getCode(){
    FILE *file;
    const char *codePath = "D:\\Users\\DariusYoung\\Documents\\GitHub\\PL-0-Lexer-parser\\test3.txt";
    file = fopen(codePath, "r");
    fseek(file, 0, SEEK_END);
    int file_size;
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    Code = (char *)malloc(file_size * sizeof(char));
    fread(Code, file_size, sizeof(char), file);
    fclose(file);
}

void error(int errorNum){
    switch (errorNum){
        case 1: printf("Line %d error: 嵌套层数超过上限3\n", Line); break;
        case 2: printf("Line %d error: 缺少；\n", Line); break;
        case 3: printf("Line %d error: const后未定义变量名\n", Line); break;
        case 4: printf("Line %d error: 常量定义应使用'='\n", Line); break;
        case 5: printf("Line %d error: CONSTANT should be a number\n", Line); break;
        case 6: printf("Line %d error: 缺少过程名\n", Line); break;
        case 7: printf("Line %d error: var后未定义变量名\n", Line); break;
        case 8: printf("Line %d error: 变量未定义\n", Line); break;
        case 9: printf("Line %d error: constant值不允许更改\n", Line); break; //
        case 10: printf("Line %d error: 变量赋值缺少:=符号\n", Line); break;
        case 11: printf("Line %d error: 过程名未定义\n", Line); break;
        case 12: printf("Line %d error: call后应接过程名\n", Line); break;
        case 13: printf("Line %d error: 语句末尾缺少; 或end\n", Line); break;
        case 14: printf("Line %d error: 缺少then\n", Line); break;
        case 15: printf("Line %d error: 缺少do\n", Line); break;
        case 16: printf("Line %d error: 缺少(\n", Line); break;
        case 17: printf("Line %d error: 缺少)\n", Line); break;
        case 18: printf("Line %d error: 因子定义错误\n", Line); break;
        case 19: printf("Line %d error: 因子不能为过程\n", Line); break;
        case 20: printf("Line %d error: 关系符号错误\n", Line); break;
        default: printf("未定义错误/n");
    }
    sumOfError++;
}

void addTable(enum object KIND,int* TX,int lev,int* DX){
    strcpy(table[* TX].name, token);
    table[*TX].kind = KIND;
    switch (KIND){
        case CONSTANT:
            table[(*TX)].VAL=sum;
            break;
        case VARIABLE:
            table[(*TX)].LEVEL=lev;
            table[(*TX)].ADR=(*DX);
            (*DX)++;
            break;
        case PROCEDUR: /* 过程名字 */
            table[(*TX)].LEVEL=lev;
            break;
    }
    (* TX)++;
}

int searchTable(char* idt,int tx){
    int i;
    i=tx;
    while(i>=0){
        if(strcmp(table[i].name,idt)==0){
            break;
        }
        i--;
    }
//    if(i<0){
//        cout<<"table中未找到变量"<<endl;      //调试~~~~
//    }
    return i;
}

int constdeclaration(int *TX, int lev, int *DX){
    if(sym != IDENT){
        error(3);   //const后未定义变量名
    }
    else{
        addTable(CONSTANT, TX, lev, DX);
        getSym();
    }
    if(sym != EQ){
        error(4); //常量定义错误
    }
    getSym();
    if(sym!=NUMBER){
        error(5); //CONSTANT should be a number;
        getSym();
        return 0;
    }

    getSym();
    return 0;
}

int vardeclaration(int* TX,int lev,int* DX){
    if(sym!=IDENT){
        error(7);   //var后未定义变量名

        return 0;
    }
    addTable(VARIABLE, TX, lev, DX);
    getSym();
    return 0;
}

int statement(int* TX,int lev){
    int Posi;
    switch (sym){
        case IDENT:
            if(searchTable(token,*TX) != -1){
                Posi = searchTable(token,*TX);
                if(table[Posi].kind==CONSTANT){
                    error(9);   //constant值不允许更改
                }
            }
            else{
                error(8);       //变量未定义
            }
            getSym();
            if(sym!=ASGN){
                error(10);  //变量赋值缺少:=符号
            }
            else{
                getSym();
            }
            expression(TX, lev);
            break;
        case CALLSYM:
            getSym();
            if(sym==IDENT){
                Posi=searchTable(token,*TX);
                if(Posi != -1){
                    if(table[Posi].kind!=PROCEDUR){
                        error(12);  //call后应接过程名
                    }

                }else{
                    error(11);  //过程名未定义
                }

            }else{
                error(12);  //call后应接过程名
            }
            getSym();
            break;
        case BEGINSYM:
            getSym();
            while(sym!=ENDSYM){
                statement(TX,lev);
                if(sym!=SEMICOLON){
                    Line--;
                    error(2); //缺少；
                    Line++;
                }
                else if(sym==SEMICOLON){
                    getSym();
                }
            }
            getSym();
//            statement(TX,lev);
//            while(sym==SEMICOLON){
//                getSym();
//                statement(TX,lev);
//            }
//
//            if(sym==ENDSYM){
//                getSym();
//            } else{
//                error(13);  //语句末尾缺少; 或end
//            }
            break;
        case IFSYM:
            getSym();
            condition(TX,lev);
            if(sym!=THENSYM){
                Line--;
                error(14);  //缺少then
                Line++;
            }
            else{
                getSym();
            }
            statement(TX,lev);
            break;
        case WHILESYM:
            getSym();
            condition(TX,lev);
            if(sym!=DOSYM){
                Line--;
                error(15); //缺少do
                Line++;
            }
            else{
                getSym();
            }
            statement(TX,lev);
            break;
        case READSYM:
            getSym();
            if(sym!=LPAREN){
                error(16);  //缺少(
            }
            do{
                getSym();
                if(sym==IDENT){
                    Posi=searchTable(token, *TX);
                    if(Posi == -1 ){
                        error(8);   //变量未定义
                    }

                }
                getSym();
            }while(sym==COLON);
            if(sym!=RPAREN){
                error(17);  //缺少)
            }
            else{
                getSym();
            }
            break;
        case WRITESYM:
            getSym();
            if(sym!=LPAREN){
                error(16);  //缺少(
            }
            do{
                getSym();
                expression(TX,lev);
            }while(sym==COLON);

            if(sym!=RPAREN){
                error(17);  //缺少)
            }
            getSym();
            break;
        default: break;
    }
    return 0;
}

int expression(int* TX,int lev){
    if(sym==ADDSYM || sym==SUBSYM){
        getSym();
        term(TX,lev);
    }else{
        term(TX,lev);
    }
    while(sym==ADDSYM || sym==SUBSYM){
        getSym();
        term(TX,lev);
    }
    return 0;
}

int term(int* TX,int lev){
    factor(TX,lev);
    while(sym==MULTISYM || sym==DIVSYM){
        getSym();
        factor(TX,lev);
    }
    return 0;
}

int factor(int* TX,int lev){
    if(sym==IDENT){
        int Posi=searchTable(token, *TX);
        if(Posi != -1){
            if(table[Posi].kind == PROCEDUR){
                error(19);  //因子不能为过程
            }

        }else{
            error(8);       //变量未定义
        }
        getSym();
    }else if(sym==NUMBER){
        getSym();
    }else if(sym==LPAREN){
        getSym();
        expression(TX,lev);
        if(sym!=RPAREN){
            error(17);  //缺少)
        }
        getSym();
    }else{
        error(18);  //因子定义错误
        getSym();
    }
    return 0;
}

int condition(int* TX,int lev){
    if(sym==ODDSYM){
        getSym();
        expression(TX,lev);
    }else{
        expression(TX,lev);
        switch (sym){
            case EQ: getSym(); break;
            case NE: getSym(); break;
            case LT: getSym(); break;
            case LE: getSym(); break;
            case GT: getSym(); break;
            case GE: getSym(); break;
            default: error(20);  getSym(); //关系符号错误
        }
        expression(TX,lev);
    }
    return 0;
}

int block(int lev, int tx){
//    int tx0 = tx;
    int dx = 3;
    table[tx].ADR = cx;
    if(lev>levmax) error(1); //嵌套层数超过上限3
    /*声明部分开始*/
    if(sym == CONSTSYM){
        getSym();
        do{
            constdeclaration(&tx,lev,&dx);
            while(sym == COLON) {
                getSym();
                constdeclaration(&tx,lev,&dx);
            }
            if(sym==SEMICOLON) {
                getSym();
            }
            else{
                error(2); //缺少；
            }
        }while(sym==IDENT);
    }
    if(sym == VARSYM){
        getSym();
        do{
            vardeclaration(&tx,lev,&dx);
            while(sym==COLON){
                getSym();
                vardeclaration(&tx,lev,&dx);
            }
            if(sym==SEMICOLON){
                getSym();
            }
            else{
                error(2); //缺少;
            }
        }while(sym==IDENT);
    }
    while(sym == PROCEDURESYM) {
        getSym();
        if (sym == IDENT) {
            addTable(PROCEDUR, &tx, lev, &dx);
            getSym();
        } else {
            error(6);  //缺少过程名
        }
        if (sym == SEMICOLON) {
            getSym();
        } else error(2);    //缺少;
        block(lev + 1, tx); /* 递归调用 */
        if (sym == SEMICOLON) {
            getSym();
        }
        else{
            error(2); //缺少;
        }
    }
    /*声明部分结束*/

    /*过程部分开始*/
    statement(&tx,lev);
    /*过程部分结束*/
    if(sym==PERIOD){
//        cout<<"程序结束"<<endl;
        return 0;
    }
    return 0;
}

int main(){
    Sleep(3000);
    getCode();
    getSym();
    block(0, 0);
    cout<<"共 "<<sumOfError<<" 处错误"<<endl;
    return 0;
}

