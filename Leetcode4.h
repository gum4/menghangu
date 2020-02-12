//
//  Leetcode4.h
//  leetcode 4
//
//  Created by 顾孟瀚 on 12/16/19.
//  Copyright © 2019 顾孟瀚. All rights reserved.
//

#ifndef Leetcode4_h
#define Leetcode4_h

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <map>

using namespace std;
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

void storing (TreeNode* root, map<TreeNode*, vector<TreeNode*> > & Graph, map<TreeNode*, int> & flag){
    if (!root) return;
    flag[root]=0;
    if (root->left) Graph[root].push_back(root->left);
    if (root->right) Graph[root].push_back(root->right);
    if (root->left) storing(root->left,Graph,flag);
    if (root->right) storing(root->right,Graph,flag);
    return;
}
void allZeroSubTree(TreeNode* T, bool& judge){
    if (!T) return;
    if (T->val==1) judge=false;
    if (T->left) allZeroSubTree(T->left,judge);
    if (T->right) allZeroSubTree(T->right,judge);
    return;
}
//judges whetehr a node is valid
bool validNode(TreeNode* root, int target){
    //if (!root) return;
    if (root->val==target){
        if (root->left&&!root->right){
            return validNode(root->left,target);
        }
        else if (!root->left&&root->right){
            return validNode(root->right,target);
        }
        else if (root->left&&root->right){
            return validNode(root->left,target)||validNode(root->right,target);
        }
        else{
            return false;
        }
    }
    return true;
}
map<char, vector<int>> indexes(string S){
    map<char,vector<int>> M, Res;
    map<char,vector<int>>::iterator itr;
    for (unsigned int i=0;i<S.length();i++){
        M[S[i]].push_back(i);
    }
    for (itr=M.begin();itr!=M.end();itr++){
        Res[itr->first].push_back(itr->second[0]);
        Res[itr->first].push_back(itr->second[itr->second.size()-1]);
    }
    return Res;
}
//area stores the maximum area of an island at a certain point
void areaAtPoint(vector<vector<char>>& matrix, int & i, int & j, int & area){
    int i1=i-1,i2=i+1,j1=j-1,j2=j+1;
    area++;
    matrix[i][j]='0';
    //go up
    if (!(i1<0||i1>matrix.size()-1||j<0||j>matrix[0].size()-1)){
        if (matrix[i1][j]=='1')
            areaAtPoint(matrix,i1,j,area);
    }
    //go down
    if (!(i2<0||i2>matrix.size()-1||j<0||j>matrix[0].size()-1)){
        if (matrix[i2][j]=='1')
            areaAtPoint(matrix,i2,j,area);
    }
    //go left
    if (!(i<0||i>matrix.size()-1||j1<0||j1>matrix[0].size()-1)){
        if (matrix[i][j1]=='1')
            areaAtPoint(matrix,i,j1,area);
    }
    //go right
    if (!(i<0||i>matrix.size()-1||j2<0||j2>matrix[0].size()-1)){
        if (matrix[i][j2]=='1')
            areaAtPoint(matrix,i,j2,area);
    }
    return;
}
void expand (map<TreeNode*, vector<TreeNode*> > & Graph, map<TreeNode*, int> & flag){
    map<TreeNode*, vector<TreeNode*> >::iterator itr;
    for (itr=Graph.begin();itr!=Graph.end();itr++){
        for (unsigned int i=0;i<itr->second.size();i++){
            if (flag[itr->second[i]]==0)
                Graph[itr->second[i]].push_back(itr->first);
        }
        flag[itr->first]=1;
    }
}

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};
class Matrix {
public:
    //Matrix();
    Matrix(vector<vector<int> >& V){
        vector<int> tmp=vector<int>(V[0].size(),0);
        content=vector<vector<int> > (V.size(),tmp);
        vector<char> tmp1=vector<char>(V[0].size(),'-');
        display=vector<vector<char> > (V.size(),tmp1);
        for (unsigned int i=0;i<V.size();i++){
            for (unsigned int j=0;j<V[0].size();j++){
                content[i][j]=V[i][j];
            }
        }
    };
    void destroy(){
        content.clear();
        vector<char> tmp1=vector<char>(content[0].size(),'-');
        display=vector<vector<char> > (content.size(),tmp1);
    }
    //void resize(int rows, int cols);
    int& at(int row, int col){
        return content[row][col];
    };
    bool outOfBound(int  R, int  C){
        return (R<0||R>content.size()||C<0||C>content[0].size());
    };
    bool noMineAround(int R, int C){
        if (!outOfBound(R-1,C)&&content[R-1][C]==9) return false;
        if (!outOfBound(R+1,C)&&content[R+1][C]==9) return false;
        if (!outOfBound(R,C-1)&&content[R][C-1]==9) return false;
        if (!outOfBound(R,C+1)&&content[R][C+1]==9) return false;
        return true;
    }
    void showContent(){
        for (unsigned int i=0;i<content.size();i++){
            for (unsigned int j=0;j<content[i].size();j++){
                cout<<content[i][j]<<" ";
            }
            cout<<endl;
        }
    }
    
    void showDisplay(){
        for (unsigned int i=0;i<display.size();i++){
            for (unsigned int j=0;j<display[i].size();j++){
                cout<<display[i][j]<<" ";
            }
            cout<<endl;
        }
    }
    void zero(int & R, int & C);
    void play(int & R, int & C);
    
    
    
private:
    vector<vector<int> > content;
    vector<vector<char> > display;
};

void Matrix::zero(int & R, int & C){
    
}
void Matrix::play(int & R, int & C){
    if (content[R][C]==9){
        cout<<"That's a mine, you are dead!"<<endl;
        return;
    }
    else if (content[R][C]!=0){
        display[R][C]=content[R][C]+'0';
        showDisplay();
    }
    else{
        zero(R,C);
        showDisplay();
    }
}
////////////////////55////////////////////////
bool validIndex(vector<int>& nums, int index, int target){
    return nums[index]>=(target-index);
}


//此方法适用于找到一种解的递归情形
void jump (vector<int>& nums, int target, vector<int>& steps, vector<vector<int> >& STEPS, int & con){
    if (steps[steps.size()-1]==0) {
        //cout<<1<<endl;
        STEPS.push_back(steps);
        steps.clear();
        steps.push_back(target);
        return;
    }
    for (unsigned int i=0;i<con;i++){
        if(validIndex(nums,i,con)){
            steps.push_back(i);
            con=i;
            jump(nums,target,steps,STEPS,con);
        }
    }
    return;
}



int inVec(int i, vector<int> V){
    vector<int>::iterator itr=find(V.begin(), V.end(),i);
    if(itr==V.end()) return 0;
    return itr-V.begin()+1;
}
string fractionToDecimal(int numerator, int denominator) {
    
    //5/7
    string decimal="";
    if (numerator==0) return "0";
    if (denominator == 0) return decimal;
    bool minus=(numerator<0&&denominator>0)||(numerator>0&&denominator<0);
    long long num = abs((long long)numerator);
    long long den = abs((long long)denominator);
    
    vector<int> Pre;
    string Real="",Res;
    int Part1=num/den; //0
    int Part2=num-Part1*den;//5
    if (Part2==0){
        if (minus) Res+="-";
        Res+=to_string(Part1);
        return Res;
    }
    int Remain=Part2*10;//50
    int P1=Remain/den;//7
    int P2=Remain-P1*den;//1
    if (P2==0){
        if (minus) Res+="-";
        Res+=to_string(Part1);
        Res+=".";
        Res+=to_string(P1);
        return Res;
    }
    Pre.push_back(Part2); //5
    if (P2==Part2){
        Res+=to_string(Part1);
        Res+=".";
        Res+="(";
        Res+=to_string(P1);
        Res+=")";
        return Res;
    }
    Pre.push_back(P2); //5,1
    string not_recur,recur;
    Real+=to_string(P1); //7
    while (true){
        Remain=P2*10; //10->30->20->60->40
        P1=Remain/den;//1->4->2->8->5
        P2=Remain-P1*den;//3->2->6->4->5
        Real+=to_string(P1);//7,1,4,2,8,5
        if (inVec(P2,Pre)!=0){
            int pos=inVec(P2,Pre);
            not_recur=Real.substr(0,pos-1);
            recur=Real.substr(pos-1,Real.length()-pos+1);
            break;
        }
        Pre.push_back(P2);//5,1,3,2,6,4
    }
    if (minus) Res+="-";
    Res+=to_string(Part1);
    Res+=".";
    Res+=not_recur;
    if (recur[recur.length()-1]!='0'){
        Res+="(";
        Res+=recur;
        Res+=")";
    }
    return Res;
}


int ASC2_hash (char * str, int array_size){
    int ASC=0;
    int i=0;
    for (i=0;*(str+i)!='\0';i++){
        ASC+=(int)(*(str+i));
    }
    return ASC % array_size;
}
vector<int> pathInZigZagTree(int label) {
    int layers=(int)log2(label); //3
    //index记录目标是从左往右数的第几个元素
    int index=label-(pow(2,layers)-1); //4
    bool from_left=layers%2==0; //false
    //第layers+1层index个，如果from_left为true，则为从左数
    int at_layer=pow(2,layers);
    //at_layer记录label所在层共有几个节点
    //更新index
    if (!from_left){
        index=pow(2,layers)+1-index;
    }
    vector<int> Res;
    int L=1;
    int I=1;
    bool f_l=true;
    Res.push_back(I);
    while (at_layer>1){
        f_l=!f_l;
        L++; //2
        if (index<=at_layer/2){//left child
            at_layer/=2;
            I*=2;
            if (f_l) {
                Res.push_back(I);
            }
            else {
                int loc=I-(pow(2,L-1)-1); //1
                int J=pow(2,L)-loc; //4-2
                Res.push_back(J);
                //I=J;
            }
        }
        else{//right child
            index-=at_layer/2;
            at_layer/=2;
            I=I*2+1;
            if (f_l){
                Res.push_back(I);
            }
            else{
                int loc=I-(pow(2,L-1)-1); //1
                int J=pow(2,L)-loc; //4-2
                Res.push_back(J);
                //I=J;
            }
        }
    }
    
    return Res;
    
}
ListNode* copyList(ListNode* list){
    ListNode* newList=new ListNode(list->val);
    ListNode* record=newList;
    ListNode* tmp=list->next;
    while(tmp){
        newList->next=new ListNode(tmp->val);
        tmp=tmp->next;
        newList=newList->next;
    }
    return record;
}

int reverseList(ListNode*& list){
    int count=0;
    ListNode* prev=NULL;
    ListNode* cur=list;
    ListNode* next=cur->next;
    while (cur->next){
        cur->next=prev;
        //next->next=cur;
        prev=cur;
        cur=next;
        next=cur->next;
        count++;
    }
    cur->next=prev;
    list=cur;
    return count;
}

//sortList将链表从小到大排序
void sortList(ListNode* & list){
    ListNode* head=list;
    ListNode* tail_judge=NULL;
    while(tail_judge!=head){
        while (list->next!=tail_judge){
            ListNode* tmp1;
            if (list->next->next!=tail_judge&&list->next->val>list->next->next->val){
                ListNode* tmp=list->next;
                tmp1=list->next->next;
                tmp->next=tmp1->next;
                tmp1->next=tmp;
                list->next=tmp1;
            }
            int cur;
            if (list->val>tmp1->val){
                cur=tmp1->val;
                tmp1->val=list->val;
                list->val=cur;
            }
            list=list->next;
            
        }
        
        tail_judge=list;
        list=head;
        
    }
    list=head;
}

//////////////
//within judges whether bracket v2 are included in v1
bool within (vector<int> v1, vector<int> v2){
    return v1[0]<=v2[0]&&v1[1]>=v2[1];
}
//overlap judges whether two brackets are overlapped
bool overlap1 (vector<int> v1, vector<int> v2){
    return (v1[0]<v2[0]&&v1[1]<v2[1]);
}
bool overlap2 (vector<int> v1, vector<int> v2){
    return (v1[0]>v2[0]&&v1[1]>v2[1]);
}


/////////
ListNode* mergeKLists(vector<ListNode*>& lists) {
    if(lists.size()==0) return NULL;
    //if (!lists[0]) return NULL;
    ListNode* Res=new ListNode(lists[0]->val);
    ListNode* C=Res;
    for (unsigned int i=0;i<lists.size();i++){
        if (!lists[i]) continue;
        ListNode* tmp;
        if (i==0){
            if (lists[i]->next){
                tmp=lists[i]->next;
            }
            else{
                continue;
            }
        }
        else{
            tmp=lists[i];
        }
        while (tmp){
            Res->next=new ListNode(tmp->val);
            tmp=tmp->next;
            Res=Res->next;
        }
        
    }
    sortList(C);
    return C;
}
void printList(ListNode* list){
    while(list){
        cout<<list->val<<" ";
        list=list->next;
    }
    cout<<endl;
}
#endif /* Leetcode4_h */
