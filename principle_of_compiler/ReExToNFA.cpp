#include <bits/stdc++.h>//将中缀式转化为后缀式
using namespace std;

int priority_level(char a) //返回运算符的优先级，优先级定义：闭包（*）>连接符（.）>或运算（|）>括号
{
    if (a == '(' || a == ')')
        return 1;
    else if (a == '|')
        return 2;
    else if (a == '.')
        return 3;
    else if (a == '*')
        return 4;
    else
        return 5;
}

bool is_operand(char a) //判断是否操作数，这里吧数字和字母当做操作数
{
    if (a >= '0' && a <= '9')
        return true;
    if (a >= 'a' && a <= 'z')
        return true;
    if (a >= 'A' && a <= 'Z')
        return true;
    return false;
}

/*使用a、b、c代表运算数，则下面的情况之间会省略连接符：
*a *( ab a( )( 
*/

string infixTopostfix(string &infix_expression) //把中缀转化为后缀，并且补上在中缀中省略的连接符
{
    char temp[255];
    int j = 0;
    stack<char> st;
    for (int i = 0; i < infix_expression.size(); i++)
    {
        if (is_operand(infix_expression[i]))
        {
            temp[j++] = infix_expression[i];
        }
        else
        {
            if (st.empty())
            {
                st.push(infix_expression[i]);
            }
            else
            {
                char a = st.top();
                if (priority_level(infix_expression[i]) <= priority_level(a))
                {
                    while (priority_level(a) >= priority_level(infix_expression[i]))
                    {
                        if (a != '(' && a != ')')
                            temp[j++] = a;
                        st.pop();
                        if (st.empty())
                            break;
                        else
                            a = st.top();
                    }
                    st.push(infix_expression[i]);
                }
                else
                {
                    st.push(infix_expression[i]);
                }
            }
        }
        if (i < infix_expression.size() - 1) //判断下一个符号是不是连接符
            if ((is_operand(infix_expression[i]) && infix_expression[i + 1] == '(') ||
                (is_operand(infix_expression[i]) && is_operand(infix_expression[i + 1])) ||
                (infix_expression[i] == '*' && is_operand(infix_expression[i + 1])) ||
                (infix_expression[i] == '*' && infix_expression[i + 1] == '(') ||
                (infix_expression[i] == ')' && infix_expression[i + 1] == '('))
            {
                if (st.empty())
                {
                    st.push('.');
                }
                else
                {
                    char a = st.top();
                    if (priority_level('.') <= priority_level(a))
                    {
                        while (priority_level(a) >= priority_level('.'))
                        {
                            if (a != '(' && a != ')')
                                temp[j++] = a;
                            st.pop();
                            if (st.empty())
                                break;
                            else
                                a = st.top();
                        }
                        st.push('.');
                    }
                    else
                    {
                        st.push('.');
                    }
                }
            }
    }
    while (!st.empty())
    {
        if (st.top() != '(' && st.top() != ')')
            temp[j++] = st.top();
        st.pop();
    }
    string temp0(temp);
    return temp0;
}

int main()
{
    string infix_expression;
    cin >> infix_expression; //输入中缀表达式
    string postfix_expression;
    postfix_expression = infixTopostfix(infix_expression);
    cout << '\n'
         << postfix_expression << '\n';
    system("pause");
}
