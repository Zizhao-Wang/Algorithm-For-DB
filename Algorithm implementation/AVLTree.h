//
// Created by WLSYH on 2021/6/9.
//

#ifndef ALGORITHM_IMPLEMENTATION_AVLTREE_H
#define ALGORITHM_IMPLEMENTATION_AVLTREE_H
#include<vector>
using namespace std;
const int N=300;


class AVLTree
{

    int Leftchild[N]={0};
    int Rightchild[N]={0};
    int Height[N]={0};
    int Node[N]={0};
    int index=0;

    public: AVLTree()
   {
       vector<int> Var;
       int n;
       cout<<"Please input the number of variables!\n";
       cin>>n;
       cout<<"Please input "<<n<<" variables!\n";

       int root=0;
       while (n--)
       {
           int temp;
           cin>>temp;
           Insert(root,temp);
       }
       cout<<Node[root];

   }

    public:AVLTree(vector<int> temp)
    {
        int root;
        for(int i=0;i<temp.size();i++)
        {
            Insert(root,temp[i]);
        }
        cout<<Node[root];
    }

    void update(int u)
    {
        Height[u]=max(Height[Leftchild[u]],Height[Rightchild[u]])+1;
    }

    void RightHand(int & u)
    {
        int temp=Leftchild[u];
        Leftchild[u]=Rightchild[temp], Rightchild[temp]=u;
        update(u),update(temp);
        u=temp;
    }

    void LefttHand(int & u)
    {
        int temp=Rightchild[u];
        Rightchild[u]=Leftchild[temp], Leftchild[temp]=u;
        update(u),update(temp);
        u=temp;
    }
    int GetBalance(int u)
    {
        return Height[Leftchild[u]]-Height[Rightchild[u]];
    }

    void Insert(int & u,int temp)
    {
        if(!u) u=++index, Node[u]=temp;
        else if(temp < Node[u])
        {
            Insert(Leftchild[u],temp);
            if(GetBalance(u)==2)
            {
                if(GetBalance(Leftchild[u])==1) RightHand(u);
                else
                    LefttHand(Leftchild[u]),RightHand(u);
            }
        }
        else
        {
            Insert(Rightchild[u],temp);
            if(GetBalance(u)== -2)
            {
                if(GetBalance(Rightchild[u])== -1) LefttHand(u);
                else
                    RightHand(Rightchild[u]),LefttHand(u);
            }
        }
        update(u);
    }

};





#endif //ALGORITHM_IMPLEMENTATION_AVLTREE_H
