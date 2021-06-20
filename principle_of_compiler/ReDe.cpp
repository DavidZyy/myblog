//递归下降子程序分析
//为了方便写代码，使用“E_”表示“E’”，使用“@”代表“ε”，特此声明。


#include <bits/stdc++.h>
using namespace std;

struct info_node //保存分析步骤的信息
{
    //int steps_number;//分析步骤号
    string grammer; //用到的语法
    string hv_ana;  //已经分析过的字符串
    char bi_ana;    //正在分析的字符
    string rem_str; //剩余的字符串
    info_node(/*int steps_number,*/ string grammer, string hv_ana, char bi_ana, string rem_str)
    {
        //this->steps_number = steps_number;
        this->grammer = grammer;
        this->hv_ana = hv_ana;
        this->bi_ana = bi_ana;
        this->rem_str = rem_str;
    }
};

string expression; //输入的表达式
int lookahead;     //当前待匹配字符的下标
//set<int> error_id; //匹配时出错的字符的下标
stack<info_node *> steps; //存放信息节点，用来打印
bool is_valid = true;     //输入的表达式是否合法
int steps_number = 0;     //当前步骤的编号

/****************function declaration********/
void E();

void match(char a)
{
    if (lookahead >= expression.size())//说明已经分析完成函数代码只有最后一句话有用，代码冗余。
    {
        is_valid = false;
        return;
    }
    if (expression[lookahead] != a)
    {
        //error_id.insert(lookahead);
        is_valid = false;
    }
    else
        lookahead++;
}

void M()
{
    if (lookahead >= expression.size())//说明已经分析完成
    {
        is_valid = false;
        return;
    }
    if (expression[lookahead] == '*') //这里就不用回溯了，用if判断即可，回溯当然也可以写
    {
        steps.push(new info_node(/*steps_number++,*/ "M->*", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
        match('*');
    }
    else if(expression[lookahead] == '/')
    {
        steps.push(new info_node(/*steps_number++,*/ "M->/", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
        match('/');
    }
    else
        is_valid = false;
}

void A()
{
    if (lookahead >= expression.size())//说明已经分析完成
    {
        is_valid = false;
        return;
    }
    if (expression[lookahead] == '+') //这里就不用回溯了，用if判断即可，回溯当然也可以写
    {
        steps.push(new info_node(/*steps_number++,*/ "A->+", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
        match('+');
    }
    else if(expression[lookahead] == '-')
    {
        steps.push(new info_node(/*steps_number++,*/ "A->-", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
        match('-');
    }
    else
        is_valid = false;
}

void F()
{
    if (lookahead >= expression.size())//说明已经分析完成
    {
        is_valid = false;
        return;
    }
    if (expression[lookahead] == '(') //这里就不用回溯了，用if判断即可，回溯当然也可以写
    {
        steps.push(new info_node(/*steps_number++,*/ "F->(E)", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
        match('(');
        E();
        match(')');
    }
    else if(expression[lookahead] == 'i')
    {
        steps.push(new info_node(/*steps_number++,*/ "F->i", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
        match('i');
    }
    else
        is_valid = false;
}

void T_()
{
    if(!is_valid)
        return;

    int cu_lookahead = lookahead;//记录下当前匹配的值，加这句话是为了放置类似于“i*”被误判正确。
    steps.push(new info_node(/*steps_number++,*/ "T'->MFT'", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
    M();
    F();
    T_();
    if (!is_valid) //回溯调用另一条语句
    {
        if(cu_lookahead != lookahead)//说明有字符匹配了,但是既然回溯，那么匹配的字符就不算。
            lookahead--;
        is_valid = true;
        steps.pop();
        steps.push(new info_node(/*steps_number++,*/ "T'->ε", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
    }
}
void T()
{
    if(!is_valid)
        return;
    steps.push(new info_node(/*steps_number++,*/ "T->FT'", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
    F();
    T_();
}

void E_() //带回溯
{
    if(!is_valid)
        return;

    int cu_lookahead = lookahead;//记录下当前匹配的值，加这句话是为了放置类似于“i*”被误判正确。
    //cout<<lookahead;
    steps.push(new info_node(/*steps_number++,*/ "E'->ATE'", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
    A();
    T();
    E_();
    if (!is_valid) //回溯调用另一条语句：E->ε
    {
        if(cu_lookahead != lookahead)//说明有字符匹配了,但是既然回溯，那么匹配的字符就不算。
            lookahead--;
        is_valid = true;
        steps.pop();
        steps.push(new info_node(/*steps_number++,*/ "E'->ε", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
    }
}

void E()
{
    if(!is_valid)
        return;
    steps.push(new info_node(/*steps_number++,*/ "E->TE'", expression.substr(0, lookahead), expression[lookahead], expression.substr(lookahead)));
    T();
    E_();

}

void print_table()
{
    while(!steps.empty())
    {
        cout<<setiosflags(ios::left)<<setw(15)<<steps.top()->grammer<<setw(15)<<steps.top()->hv_ana<<setw(15)<<steps.top()->bi_ana<<setw(15)<<steps.top()->rem_str<<'\n';
        /*printf("%*s",20,steps.top()->grammer);
        printf("%*s",20,steps.top()->hv_ana);
        printf("%*s",20,steps.top()->bi_ana);
        printf("%*s",20,steps.top()->rem_str);*/
        steps.pop();
    }
}

int main()
{
    //cout<<lookahead;
    cin >> expression;
    E();
    if(lookahead == expression.size())
        is_valid = true;
    else
        is_valid = false;
    if(is_valid)
        cout<<"表达式合法！"<<'\n';
    else
        cout<<"表达式不合法！"<<'\n';
    cout<<setiosflags(ios::left)<<setw(15)<<"文法"<<setw(15)<<"已分析"<<setw(15)<<"正分析"<<setw(15)<<"未分析"<<'\n';
    if(is_valid)
        print_table();
    system("pause");
}
