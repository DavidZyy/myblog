#include<bits/stdc++.h>
using namespace std;

//保留字
string reserved_keywords[] = {"const", "int", "void", "if", "while", "break", "return", "continue", "main"};
set<string> reserved_keywords_set(reserved_keywords, reserved_keywords + 9);

//运算符
string operational_characters[] = {"+", "-", "*", "/", "%", "==", "!=", "<", ">", "<=", ">=", "!", "&&", "||", "[", "]", "++", "--", "=", "><"};
set<string> operational_characters_set(operational_characters, operational_characters + 20);

//分隔符
string separators[] = {";", "{", "}", "(", ")"};
set<string> separators_set(separators, separators +5);

//自定义标识符
set<string> identifiers_set;

//保存处理好注释的一行字符串
char dele_annotation[255];
//保存处理好退格符的字符串
char dele_tab[255];

bool isdigit(char ch)
{
    if (ch >= '0'&&ch <= '9')
        return true;
    return false;
}

bool isletter(char ch)
{
    if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
        return true;
    return false;
}

//返回字符ch的类型
int char_type(char ch)
{
    if(isdigit(ch))
        return 1;
    if(isletter(ch))
        return 2;
    if(ch == '_')
        return 3;
    string ch1(1, ch);//把char转为为string
    if(operational_characters_set.find(ch1) != operational_characters_set.end())
        return 4;
    if(separators_set.find(ch1) != separators_set.end())
        return 5;
    return 6;
}

void Handle_annotation(char *src) //把src中除了注释的内容都放到dele_annotation里面
{
    int j = 0;
    memset(dele_annotation, 0, 255);
    for(int i = 0; i < strlen(src); i++)
    {
        if(src[i] == '/')//错误一原来for里写的是i < strlen(src)-1 
            if(src[i+1] == '/')
                return;
        dele_annotation[j++] = src[i];
    }
}

void Handle_tab(char *src) //去掉tab
{
    int j = 0;
    memset(dele_tab, 0, 255);
    for(int i = 0; i < strlen(src); i++)
    {
        if(src[i] == '\t')
            continue;
        dele_tab[j++] = src[i];
    }
}

int main()
{
    char one_row_char[255] = {};//存放一次性读取的文件中一行的字符
    bool is_annotation = false;//当前行是否在注释内
    char *str = NULL;//按照行中的空格分隔行，存放分隔后的字符串
    char delims[] = " ";//空格作为分割行的标志
    char temp[255];//用来暂存同一个str中连续的（字符，数字，下划线），（运算符），（分隔符）
    int row_number = 0;//记录行号
    freopen("source.txt", "r", stdin);
    freopen("result.txt", "w", stdout);

    while(gets(one_row_char) != NULL)
    {
        row_number++;
        Handle_tab(one_row_char);//处理tab
        Handle_annotation(dele_tab);//处理注释
        str = strtok(dele_annotation, delims);//根据空格分隔dele_annotation，得到字符串。
        while(str != NULL)
        {
            //cout<<str<<'\n';
            for(int i = 0; i < strlen(str); i++)
            {
                memset(temp, 0, 255);
                if(str[i] == '/' && str[i+1] == '*') //开启注释
                {
                    is_annotation = true;
                }

                if(str[i] == '*' && str[i+1] == '/') //取消注释
                {
                    is_annotation = false;
                    i++;
                    i++;
                    if(i >= strlen(str))
                        break;
                }

                if(char_type(str[i]) == 5 && !is_annotation) //分隔符
                {
                    printf("<分界符，%c>\n", str[i]);
                }
                else if(char_type(str[i]) == 4 && !is_annotation) //运算符
                {
                    if(char_type(str[i+1]) == 4) //运算符有两个符号
                    {
                        temp[0] = str[i];
                        temp[1] = str[i+1];
                        i++;
                        printf("<运算符，%s>\n", temp);
                    }
                    else //运算符有一个符号
                    {
                        printf("<运算符，%c>\n", str[i]);
                    }
                }
                else if(!is_annotation) //关键字、标识符、常量（数字，字符）
                {
                    int j = 0;
                    temp[j++] = str[i++];
                    while((char_type(str[i]) == 1 || char_type(str[i]) == 2 || char_type(str[i]) == 3) && i < strlen(str))
                    {
                        temp[j++] = str[i++];
                    }
                    if(isdigit(temp[0])) //如果第一个字符是数字，那么可能是整数（八、十、十六进制）或者非法标识符
                    {
                        if(temp[0] == '0')
                        {
                            if(strlen(temp) == 1)
                                printf("<数值型常量（十进制），%s>\n", temp);
                            else
                            {
                                if(temp[1] == 'x' || temp[1] == 'X') //十六进制
                                {
                                    printf("<数值型常量（十六进制），%s>\n", temp);
                                }
                                else //八进制
                                {
                                    printf("<数值型常量（八进制），%s>\n", temp);
                                }
                            }
                        }
                        else
                        {
                            int flag = 0;
                            for(int j = 0; j < strlen(temp); j++)
                            {
                                if(!isdigit(temp[j]))
                                {
                                    printf("ERROR IN LINE %d, ERROR TYPE: 标识符以数字开头。\n", row_number);
                                    flag = 1;
                                }
                            }
                            if(!flag)
                            printf("<数值型常量（十进制），%s>\n", temp);
                        }
                    }
                    else
                    {
                        if(reserved_keywords_set.find(temp) != reserved_keywords_set.end()) //是保留字
                        {
                            printf("<保留字，%s>\n", temp);
                        }
                        else
                        {
                            printf("<标识符，%s>\n", temp);
                        }
                    }
                    i--;//这里退出后，回到for循环还要++，所以这里先--;错误二
                }
            }
            str = strtok(NULL, delims);
        }
    }
    freopen("CON", "w", stdout);//为了后面输出到控制台，这里打开“控制台”文件。
    cout<<"已完成词法分析:"<<endl<<endl;
    freopen("result.txt", "r", stdin);//把文件里面的内容作为输入，输出到控制台。
    while ((gets(one_row_char)) != NULL)
        cout<<one_row_char<<endl;
    while (getchar() != '\n') //这两句代码是清空输入缓冲区内容，等同于fflush(stdin);
        continue;
    getchar();
    system("pause");
    return 0;
}
