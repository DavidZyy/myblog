//注：使用符号“@”代替符号"ε",使用X_代替符号X',使用"->"代替"→"
//输入文法：
/*
E→TE’
E’→ATE’|ε
T→FT’
T’→MFT’ |ε
F→(E) | i
A → + | -
M → * | /
*/

//实际输入符号:
/*
E->TZ
Z->ATZ|@
T->FY
Y->MFY |@
F->(E) | i
A -> + | -
M -> * | /
*/
#include <bits/stdc++.h>
using namespace std;

struct grammer_node //存放文法规则的节点
{
    string non_ter_sym;    //产生式左边的非终结符号
    vector<string> result; //产生式结果
};

vector<grammer_node *> all_gram; //存放所有的产生式
char non_ter_sym = 'Z';          //如果需要新的非终结符号，则使用
//void handle_grammer()

//建立二维映射关系
set<char> NTS_fir_set[128];     //非终结符号的first集
set<char> ri_fir_set[128][100]; //右边候选式的first集合，因为使用二维数组来存，建立映射，所以文法不能超过10条,如果两条产生式的候选式相同,这个结构是分别存储了
set<char> NTS_foll_set[128];
set<char> selc_set[128][100]; //前一个参数填写char，表示非终结符号映射，后一个参数填写数字
int analy_table[128][20];//分析表


bool is_NTS(char a) //判断是否为非终结符号
{
    for (int i = 0; i < all_gram.size(); i++)
    {
        if (all_gram[i]->non_ter_sym[0] == a)
            return true;
    }
    return false;
}

void printgram(vector<grammer_node *> &all_gram)
{
    cout << '\n';
    for (auto it = all_gram.begin(); it != all_gram.end(); it++)
    {
        //cout<<'1'<<'\n';
        cout << (*it)->non_ter_sym << '\t';
        for (auto it1 = (*it)->result.begin(); it1 != (*it)->result.end(); it1++)
        {
            //cout<<'2'<<'\n';
            cout << *it1 << '\t';
        }
        cout << '\n';
    }
}

void ex_com_le() //提取左公共因子  找到一组“最大”左公共因子,因为node只有两个，一个产生式只有一种类型的左公共可以，有两个以上的还要改代码
{
    int n = all_gram.size();
    for (int i = n - 1; i >= 0; i--) //遍历“原先”all_gram“中的每个产生式,处理左公共因子
    {
        grammer_node *node1 = new grammer_node; //存放有公共左因子的产生式删减后的产生式
        vector<grammer_node *> new_pro;         //存放新的产生式
        //grammer_node* node2 = new grammer_node;
        string min_com_le;                   //当前最小左公共因子
        int num_mcl;                         //当前最小左公共因子的数量
        int a1 = all_gram[i]->result.size(); //当前产生式候选式的数量
        int tag[a1];                         //用来表示此候选式是否已经消除公共因子
        memset(tag, 0, sizeof(tag));
        int flag = 0;                //此产生式是否有公共左因子
        for (int j = 0; j < a1; j++) //考虑有公共左因子的node删除掉，再放一个新的进去。扫描所有候选式
        {
            if (tag[j])
                continue;
            int max_com_le_num = 0;                              //记录有公共左因子的候选式的数量
            int k;                                               //
            for (k = 1; k <= all_gram[i]->result[j].size(); k++) //枚举最大左公共因子的长度
            {
                string str1 = all_gram[i]->result[j].substr(0, k);
                int com_le_num = 0; //公共左因子的候选式的数量
                for (int q = 0; q < a1; q++)
                {
                    if (all_gram[i]->result[q].size() >= k)
                        if (all_gram[i]->result[q].substr(0, k) == str1)
                        {
                            tag[q] = 1;
                            com_le_num++;
                        }
                }
                if (max_com_le_num <= com_le_num)
                    max_com_le_num = com_le_num;
                else
                    break;
            }
            k = k - 1; //长度
            grammer_node *node2 = new grammer_node;
            if (max_com_le_num > 1) //有公共左因子,当前for循环退出后删掉当前这条产生式，在这里产生新的产生式
            {
                flag = 1;
                node1->non_ter_sym = all_gram[i]->non_ter_sym;
                node1->result.push_back(all_gram[i]->result[j].substr(0, k) + non_ter_sym);

                node2->non_ter_sym = non_ter_sym;
                for (int q = 0; q < a1; q++)
                {
                    if (all_gram[i]->result[q].substr(0, k) == all_gram[i]->result[j].substr(0, k))
                    {
                        if (all_gram[i]->result[q].size() == k)
                            node2->result.push_back("@"); //表示空
                        else
                            node2->result.push_back(all_gram[i]->result[q].substr(k));
                    }
                }
                non_ter_sym--;
                new_pro.push_back(node2);
            }
            else //没有公共因子的候选式，也要放进新的node里面
            {
                node1->result.push_back(all_gram[i]->result[j]);
                delete node2;
            }
        }
        if (flag == 1)
        {
            all_gram.erase(all_gram.begin() + i);
            all_gram.push_back(node1);
            for (auto it = new_pro.begin(); it != new_pro.end(); it++)
                all_gram.push_back(*it);
        }
        else
        {
            delete node1;
            /*for(auto it = new_pro.end()-1; it >= new_pro.begin(); it--)
                delete (*it);*/
        }
    }
}

void eli_le_rec(vector<grammer_node *> &all_gram) //消除左递归,候选式中不会出现多个左递归，因为已经提取过公共左因子
{
    int n = all_gram.size();
    for (int i = n - 1; i >= 0; i--) //双重for循环把间接左递归转化为直接左递归
    {
        //一开始就有左递归，直接消除
        grammer_node *node1 = new grammer_node;
        grammer_node *node2 = new grammer_node;
        int flag = 0;
        node1->non_ter_sym = all_gram[i]->non_ter_sym;
        for (auto it = all_gram[i]->result.begin(); it != all_gram[i]->result.end(); it++)
        {
            if ((*it)[0] != all_gram[i]->non_ter_sym[0])
            {
                node1->result.push_back((*it) + non_ter_sym);
            }
        }
        node2->non_ter_sym = non_ter_sym;
        for (auto it = all_gram[i]->result.begin(); it != all_gram[i]->result.end(); it++)
        {
            if ((*it)[0] == all_gram[i]->non_ter_sym[0])
            {
                flag = 1;
                node2->result.push_back((*it).substr(1) + non_ter_sym);
                break;
            }
        }
        node2->result.push_back("@");
        if (flag)
        {
            non_ter_sym--;
            all_gram.erase(all_gram.begin() + (i));
            all_gram.push_back(node1);
            all_gram.push_back(node2);
        }
        //cout<<all_gram[i]->non_ter_sym<<'\n';
        for (int j = n - 1; j > i; j--)
        {
            grammer_node *temp1 = new grammer_node;
            int flag = 0;
            for (auto it = all_gram[i]->result.end() - 1; it >= all_gram[i]->result.begin(); it--)
            {
                if ((*it)[0] == all_gram[j]->non_ter_sym[0])
                {
                    //cout<<*it;
                    for (auto it1 = all_gram[j]->result.end() - 1; it1 >= all_gram[j]->result.begin(); it1--)
                        temp1->result.push_back((*it1) + (*it).substr(1));
                    all_gram[i]->result.erase(it);
                    for (auto it2 = temp1->result.begin(); it2 != temp1->result.end(); it2++)
                        all_gram[i]->result.push_back(*it2);
                    break;
                }
            }
        }
        //经过代换产生了左递归，再消除
        //printgram();
        //消除直接左递归
        //int n = all_gram.size();//直接左递归在最后一个产生式
        node1 = new grammer_node;
        node2 = new grammer_node;
        flag = 0;
        node1->non_ter_sym = all_gram[i]->non_ter_sym;
        for (auto it = all_gram[i]->result.begin(); it != all_gram[i]->result.end(); it++)
        {
            if ((*it)[0] != all_gram[i]->non_ter_sym[0])
            {
                node1->result.push_back((*it) + non_ter_sym);
            }
        }
        node2->non_ter_sym = non_ter_sym;
        for (auto it = all_gram[i]->result.begin(); it != all_gram[i]->result.end(); it++)
        {
            if ((*it)[0] == all_gram[i]->non_ter_sym[0])
            {
                flag = 1;
                node2->result.push_back((*it).substr(1) + non_ter_sym);
                break;
            }
        }
        node2->result.push_back("@");
        if (flag)
        {
            non_ter_sym--;
            all_gram.erase(all_gram.begin() + (i));
            all_gram.push_back(node1);
            all_gram.push_back(node2);
        }
    }
}

void cons_fir_set() //构建first集合
{
    //先运行消除左递归函数，则每个候选式开头的非终结符号被替换，都为终结符号,但是这样文法就变了，不可。
    int n = all_gram.size();
    vector<grammer_node *> all_gram1;
    for (int i = 0; i < n; i++) //把all_gram复制一份到all_gram1
    {
        grammer_node *node = new grammer_node;
        node->non_ter_sym = all_gram[i]->non_ter_sym;
        for (auto it = all_gram[i]->result.begin(); it != all_gram[i]->result.end(); it++)
            node->result.push_back(*it);
        all_gram1.push_back(node);
    }
    eli_le_rec(all_gram1);
    //printgram(all_gram1);
    for (int i = 0; i < n; i++)
    {
        for (auto it = all_gram1[i]->result.begin(); it != all_gram1[i]->result.end(); it++)
        {
            NTS_fir_set[all_gram[i]->non_ter_sym[0]].insert((*it)[0]);
        }
    }

    //构建候选式的first集
    for (int i = 0; i < n; i++)
    {
        int m = all_gram[i]->result.size();
        for (int j = 0; j < m; j++) //枚举第i条产生式的所有候选式
        {
            int m1 = all_gram[i]->result[j].size();
            for (int k = 0; k < m1; k++) //枚举候选式的字符
            {
                if (is_NTS(all_gram[i]->result[j][k])) //是非终结符号
                {
                    for (auto it = NTS_fir_set[all_gram[i]->result[j][k]].begin(); it != NTS_fir_set[all_gram[i]->result[j][k]].end(); it++)
                    {
                        if (*it != '@')
                            ri_fir_set[all_gram[i]->non_ter_sym[0]][j].insert(*it);
                    }
                    if (NTS_fir_set[all_gram[i]->result[j][k]].find('@') == NTS_fir_set[all_gram[i]->result[j][k]].end())
                    {
                        break;
                    }
                }
                else
                {
                    ri_fir_set[all_gram[i]->non_ter_sym[0]][j].insert(all_gram[i]->result[j][k]);
                    break;
                }
            }
        }
    }
}

void cons_foll_set() //构建follow集
{
    NTS_foll_set[all_gram[0]->non_ter_sym[0]].insert('$'); //文法开始符号的follow集中插入$，作为句子的结束标志
    for (int i = 0; i < all_gram.size(); i++)              //枚举非终结符号
    {
        char obj = all_gram[i]->non_ter_sym[0];
        for (int j = 0; j < all_gram.size(); j++) //双重for循环枚举每个候选式，从每个候选式中寻找非终结符号
        {
            for (int k = 0; k < all_gram[j]->result.size(); k++)
            {
                int n1 = all_gram[j]->result[k].size();
                for (int q = 0; q < n1; q++) //枚举候选式的每个符号
                {
                    if (obj == all_gram[j]->result[k][q]) //在候选式中找到了非终结符号
                    {
                        if (q == n1 - 1) //位于候选式的最后一个
                        {
                            for (auto it = NTS_foll_set[all_gram[j]->non_ter_sym[0]].begin(); it != NTS_foll_set[all_gram[j]->non_ter_sym[0]].end(); it++)
                            {
                                NTS_foll_set[all_gram[i]->non_ter_sym[0]].insert(*it);
                            }
                        }
                        else
                        {
                            if (!is_NTS(all_gram[j]->result[k][q + 1])) //如果后一个符号是终结符号
                            {
                                NTS_foll_set[all_gram[i]->non_ter_sym[0]].insert(all_gram[j]->result[k][q + 1]);
                            }
                            else //如果后一个符号是非终结符号
                            {
                                for (auto it = NTS_fir_set[all_gram[j]->result[k][q + 1]].begin(); it != NTS_fir_set[all_gram[j]->result[k][q + 1]].end(); it++)
                                {
                                    if (*it != '@')
                                        NTS_foll_set[all_gram[i]->non_ter_sym[0]].insert(*it);
                                }
                                if (NTS_fir_set[all_gram[j]->result[k][q + 1]].find('@') != NTS_fir_set[all_gram[j]->result[k][q + 1]].end()) //非终结符号能推出空
                                {
                                    for (auto it = NTS_foll_set[all_gram[j]->non_ter_sym[0]].begin(); it != NTS_foll_set[all_gram[j]->non_ter_sym[0]].end(); it++)
                                    {
                                        NTS_foll_set[all_gram[i]->non_ter_sym[0]].insert(*it);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//构建select集
void cons_selec_set()
{
    for (int i = 0; i < all_gram.size(); i++) //枚举非终结符号
    {
        char obj = all_gram[i]->non_ter_sym[0];
        for (int j = 0; j < all_gram[i]->result.size(); j++) //双重for循环枚举每个候选式，从每个候选式中寻找非终结符号
        {
            if (ri_fir_set[all_gram[i]->non_ter_sym[0]][j].find('@') != ri_fir_set[all_gram[i]->non_ter_sym[0]][j].end())
            {
                for(auto it = NTS_foll_set[all_gram[i]->non_ter_sym[0]].begin(); it != NTS_foll_set[all_gram[i]->non_ter_sym[0]].end(); it++)
                    selc_set[all_gram[i]->non_ter_sym[0]][j].insert(*it);
            }
            for (auto it = ri_fir_set[all_gram[i]->non_ter_sym[0]][j].begin(); it != ri_fir_set[all_gram[i]->non_ter_sym[0]][j].end(); it++)
            {
                if (*it != '@')
                    selc_set[all_gram[i]->non_ter_sym[0]][j].insert(*it);
            }
        }
    }
}

void cons_analy_table()//构建分析表
{
    memset(analy_table, -1, sizeof(analy_table));
    for (int i = 0; i < all_gram.size(); i++)
    {
        char obj = all_gram[i]->non_ter_sym[0];
        for (int j = 0; j < all_gram[i]->result.size(); j++)
        {
            for(auto it = selc_set[obj][j].begin(); it != selc_set[obj][j].end(); it++)
            {
                analy_table[obj][*it] = j;
            }
        }
    }
}

void parse(string expression)//分析输入的字符串并输出
{
    int step_numb = 0;//当前步骤数
    cout<<setiosflags(ios::left)<<setw(15)<<"步骤"<<setw(15)<<"已匹配"<<setw(15)<<"栈"<<setw(15)<<"输入串"<<setw(15)<<"动作"<<'\n';
    int cur_idx = 0;//当前正在分析的字符串的下标
    string analy_stack = all_gram[0]->non_ter_sym + "$";//用字符串实现分析栈，便于输出。
    //cout<<analy_stack;
    while(analy_stack.size())
    {
        if(analy_stack[0] == expression[cur_idx])//如果栈顶和当前要分析的字符相同
        {
            cout<<setiosflags(ios::left)<<setw(15)<<step_numb<<setw(15)<<expression.substr(0, cur_idx)<<setw(15)<<analy_stack<<setw(15)<<expression.substr(cur_idx)<<setw(15)<<expression[cur_idx]<<'\n';
            step_numb++;
            cur_idx++;
            analy_stack = analy_stack.substr(1);
        }
        else
        {
            int i;
            for(i = 0; i < all_gram.size(); i++)
            {
                if(all_gram[i]->non_ter_sym[0] == analy_stack[0])
                    break;
            }
            string str1 = all_gram[i]->result[analy_table[analy_stack[0]][expression[cur_idx]]];
            cout<<setiosflags(ios::left)<<setw(15)<<step_numb<<setw(15)<<expression.substr(0, cur_idx)<<setw(15)<<analy_stack<<setw(15)<<expression.substr(cur_idx)<<analy_stack[0]<<"->"<<str1<<'\n';
            step_numb++;
            analy_stack = analy_stack.substr(1);
            if(str1 != "@")
                analy_stack = str1 + analy_stack;
        }
    }
}

int main()
{
    string expression;
    cin>>expression;
    //文法从文件里读入
    freopen("D:\\Programming\\vscode\\Vs_Code_Cpp\\grammer.txt", "r", stdin);
    char one_row[255] = {}; //文件的一行，表示一个产生式
    char delims[] = " |>";
    char *str = NULL;
    while (gets(one_row) != NULL)
    {
        grammer_node *p = new grammer_node;
        //cout<<one_row<<'\n';
        int k = 0;
        str = strtok(one_row, delims);
        while (str != NULL)
        {
            //cout << str << '\n';
            if (!k)
                p->non_ter_sym = str;
            else
                p->result.push_back(str);
            k++;
            str = strtok(NULL, delims);
        }
        all_gram.push_back(p);
    }
    //cout<<'\n';
    //printgram(all_gram);
    ex_com_le();
    //printgram(all_gram);
    eli_le_rec(all_gram);
    //printgram(all_gram);
    cons_fir_set();

    cons_foll_set();
    cons_selec_set();
    cons_analy_table();
    //printgram(all_gram);
    parse(expression);
    fclose(stdin);

    //输出非终结符号的first集
    /*for(int i = 0; i < all_gram.size(); i++)
    {
        cout<<all_gram[i]->non_ter_sym[0]<<':';
        for(auto it = NTS_fir_set[all_gram[i]->non_ter_sym[0]].begin(); it != NTS_fir_set[all_gram[i]->non_ter_sym[0]].end(); it++)
        {
            cout<<*it<<'\t';
        }
        cout<<'\n';
    }*/

    /*//输出候选式的first集
    for(int i = 0; i < all_gram.size(); i++)
    {
        int n1 = all_gram[i]->result.size();
        for(int j = 0; j < n1; j++)
        {
            for(auto it = ri_fir_set[all_gram[i]->non_ter_sym[0]][j].begin(); it != ri_fir_set[all_gram[i]->non_ter_sym[0]][j].end(); it++)
            {
                cout<<*it<<'\t';
            }
            cout<<'\n';
        }
    }*/

    //输出follow集
    /*for (int i = 0; i < all_gram.size(); i++)
    {
        cout << all_gram[i]->non_ter_sym[0] << ':';
        for (auto it = NTS_foll_set[all_gram[i]->non_ter_sym[0]].begin(); it != NTS_foll_set[all_gram[i]->non_ter_sym[0]].end(); it++)
        {
            cout << *it << '\t';
        }
        cout << '\n';
    }*/

    //输出select集
    /*for(int i = 0; i < all_gram.size(); i++)
    {
        int n1 = all_gram[i]->result.size();
        for(int j = 0; j < n1; j++)
        {
            for(auto it = selc_set[all_gram[i]->non_ter_sym[0]][j].begin(); it != selc_set[all_gram[i]->non_ter_sym[0]][j].end(); it++)
            {
                cout<<*it<<'\t';
            }
            cout<<'\n';
        }
    }*/
    //cout<<analy_table['C']['$'];

    while (getchar() != '\n') //这两句代码是清空输入缓冲区内容，等同于fflush(stdin);
        continue;
    getchar();
}
