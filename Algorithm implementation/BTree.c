/***************************************************************************
    @author: Zizhao Wang
    @date:   2021/6/11
    @IDE:    Clion 2021.1
**********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "BTree.h"

#define cmp(a, b) ( (a-b >= 0) ? (1) : (0) )
#define DEBUG_BTREE

void DiskWrite(BTNode* node)
{
    int i;
    printf("Write node to disk!\n");
    for(int i=0;i<BTree_Order-1;i++)
    {
        printf("%c",node->key[i]);
    }
    printf("\n");
}

void DiskRead(BTNode** node)
{
    int i;
    for(int i=0;i<BTree_Order-1;i++)
    {
        printf("%c",(*node)->key[i]);
    }
    printf("\n");
}

void BTreePrint(const BTree tree,int layer)
{
    int i;
    BTNode * node=tree;

    if(node!=NULL)
    {
        printf("%d layer, %d node",layer,node->keynum);

        for(i=0;i<BTree_Order-1;++i)
        {
            printf("%c ",node->key[i]);
        }
        printf("\n");

        ++layer;
        for(i=0;i<=node->keynum;++i)
        {
            if(node->child[i]!=NULL)
            {
                BTreePrint(node->child[i],layer);
            }
        }
    }
    else
        printf("This is a empty B-Tree");
}


int BinarySearch(BTNode* node,int low, int high, KeyType Fkey)
{
    int mid;
    while (low<=high)
    {
        mid=low+(high-low)/2;
        if(Fkey<node->key[mid])
            high=mid-1;
        else if(Fkey>node->key[mid])
            low=mid-1;
        else
            return mid;
    }
    return -1;
}



//=======================================================insert=====================================
void BTreeSplitChild(BTNode* Parent,int index,BTNode* node)
{
    int i;
    BTNode * NewNode;
#ifdef DEBUG_BTREE
    printf("BTree_split_child!\n");
#endif
    assert(Parent && node);

    NewNode= (BTNode*)calloc(1,sizeof (BTNode));
    if(!NewNode)
    {
        printf("Error! Out of memory!\n");
        return;
    }
    NewNode->IsLeaf=node->IsLeaf;
    NewNode->keynum=BTree_Degree-1;

    for(i=0;i<NewNode->keynum;++i)
    {
        NewNode->child[i]=node->child[BTree_Degree+i];
        node->child[BTree_Degree+i]=NULL;
    }

    node->keynum=BTree_Degree-1;

    for(i=Parent->keynum;i>index;--i)
    {
        Parent->child[i+1]=Parent->child[i];
    }

    Parent->child[index+1]=NewNode;

    for(i=Parent->keynum-1;i>=index;--i)
    {
        Parent->key[i+1]=Parent->key[i];
    }

    Parent->key[index]=node->key[BTree_Degree-1];
    ++Parent->keynum;

    node->key[BTree_Degree-1]=0;

    //Write to disk!

    DiskWrite(Parent);
    DiskWrite(node);
    DiskWrite(NewNode);

}

void BTreeInsertNonfull(BTNode* node, KeyType key)
{
    int i;
    assert(node);

    if(node->IsLeaf)
    {
        i=node->keynum-1;
        while (i >= 0 && key<node->key[i])
        {
            node->key[i+1] =node -> key[i];
            --i;
        }

        node->key[i+1]=key;
        ++node->keynum;

        DiskWrite(node);
    }

    else
    {
        i=node->keynum-1;
        while (i>=0 && key<node->key[i])
            --i;
        ++i;

        DiskRead(&node->child[i]);

        if(node->child[i]->keynum == BTree_Order-1)
        {
            BTreeSplitChild(node,i,node->child[i]);

            if(key> node->key[i])
                ++i;
        }
        BTreeInsertNonfull(node->child[i],key);
    }
}


void BTreeInsert(BTree* tree, KeyType key)
{
    BTNode * node;
    BTNode * root=* tree;

#ifdef DEBUG_BTREE
    printf("BTree Insert\n");
#endif

    //Empty tree
    if(root==NULL)
    {
        root=(BTNode*) calloc(1,sizeof (BTNode));
        if(!root){
            printf("Error! Out of memory!\n");
            return;
        }

        root->IsLeaf=1;
        root->keynum=1;
        root->key[0]=key;

        *tree=root;

        DiskWrite(root);
        return;
    }

    if(root->keynum==BTree_Order-1)
    {
        node=(BTNode*) calloc(1,sizeof (BTNode));
        if(!node){
            printf("Error! Out of memory!\n");
            return;
        }

        *tree=node;
        node->IsLeaf=0;
        node->keynum=0;
        node->child[0]=root;

        BTreeSplitChild(node,0,root);
        BTreeInsertNonfull(node,key);
    }
    else
        BTreeInsertNonfull(node,key);
}



//=================================================Delete========================================
/************************************************************************************************
// 对 tree 中的节点 node 进行合并孩子节点处理.
// 注意：孩子节点的 keynum 必须均已达到下限，即均等于 BTree_D - 1
// 将 tree 中索引为 index  的 key 下移至左孩子结点中，
// 将 node 中索引为 index + 1 的孩子节点合并到索引为 index 的孩子节点中，右孩子合并到左孩子结点中。
// 并调相关的 key 和指针。
***************************************************/
void BTreeMergeChild(BTree* tree,BTNode* node,int index)
{
    int i;
    KeyType key;
    BTNode *LeftChild,*RightChild;
#ifdef DEBUG_BTREE
    printf("BTree_merge_child!\n");
#endif
    assert(tree && node && index>=0 && index<node->keynum);

    key=node->key[index];
    LeftChild=node->child[index];
    RightChild=node->child[index+1];

    assert(LeftChild && LeftChild->keynum == BTree_Degree-1 && RightChild && RightChild->keynum==BTree_Degree-1);

    LeftChild->key[LeftChild->keynum]=key;
    LeftChild->child[LeftChild->keynum+1]=RightChild->child[0];
    ++LeftChild->keynum;

    for(i=0;i<RightChild->keynum;++i)
    {
        LeftChild->key[LeftChild->keynum]=RightChild->key[i];
        LeftChild->child[LeftChild->keynum+1]=RightChild->child[i+1];
        ++LeftChild->keynum;
    }


    for(i=index;i<node->keynum-1;++i)
    {
        node->key[i]=node->key[i+1];
        node->child[i+1]=node->child[i+2];
    }

    node->key[node->keynum-1]=0;
    node->child[node->keynum]=NULL;
    --node->keynum;

    if(node->keynum==0)
    {
        if(*tree==node){
            *tree=LeftChild;
        }
        free(node);
        node=NULL;
    }

    free(RightChild);
    RightChild=NULL;
}




//The number of keywords of non-root internal nodes must be no less than BTree_Degree-1
void BTreeRecursiveRemove(BTree* tree, KeyType key)
{
    int i,j,index;
    BTNode *root=*tree;
    BTNode *node=root;
    if(!root)
    {
        printf("Failed to remove %c, it is not in the tree!\n", key);
        return;
    }

    index=0;
    while (index<node->keynum&&key>node->key[index])
    {
        ++index;
    }

    if(index<node->keynum && node->key[index]==key)
    {
        BTNode *LeftChild, *RightChild;
        KeyType LeftKey, RightKey;

        if (node->IsLeaf) {
            for (i = index; i < node->keynum; ++i)
                node->key[i] = node->key[i + 1];

            node->key[node->keynum - 1] = 0;
            --node->keynum;

            if (node->keynum == 0) {
                assert(node == *tree);
                free(node);
                *tree = NULL;
            }
            return;
        }
        /***
        * Left child moves up!
        */
        else if (node->child[index]->keynum >= BTree_Degree) {
            LeftChild = node->child[index];
            LeftKey = LeftChild->key[node->keynum - 1];
            node->key[index] = key;
            BTreeRecursiveRemove(&LeftChild, key);
        }
        /**
        * Right child moves up!
        */
        else if (node->child[index + 1]->keynum >= BTree_Degree) {
            RightChild = node->child[index + 1];
            RightKey = RightChild->key[0];
            node->key[index] = key;
            BTreeRecursiveRemove(&RightChild, key);
        } else if (node->child[index]->keynum == BTree_Degree - 1
                   && node->child[index + 1]->keynum == BTree_Degree - 1) {
            LeftChild = node->child[index];
            BTreeMergeChild(tree, node, index);
            BTreeRecursiveRemove(&LeftChild, key);
        }
    }
    else
    {
        BTNode * LeftSiBling,* RightSiBling, *child;

        child=node->child[index];
        if (!child) {
            printf("Failed to remove %c, it is not in the tree!\n", key);
            return;
        }

        if(child->keynum==BTree_Degree-1) {
            LeftSiBling = NULL;
            RightSiBling = NULL;

            if (index - 1 >= 0) {
                LeftSiBling = node->child[index - 1];
            }

            if (index - 1 <= node->keynum)
                RightSiBling = node->child[index + 1];

            if ((LeftSiBling && LeftSiBling->keynum >= BTree_Degree) ||
                (RightSiBling && RightSiBling->keynum >= BTree_Degree)) {

                int richR = 0;
                if (RightSiBling) richR = 1;
                if (RightSiBling && LeftSiBling)
                    richR = cmp(RightSiBling->keynum, LeftSiBling->keynum);

                if (RightSiBling && RightSiBling->keynum >= BTree_Degree && richR) {
                    child->key[child->keynum] = node->key[index];
                    child->child[child->keynum + 1] = RightSiBling->child[0];
                    ++child->keynum;

                    node->key[index] = RightSiBling->key[0];

                    for (j = 0; j < RightSiBling->keynum - 1; ++j) {
                        RightSiBling->key[j] = RightSiBling->key[j + 1];
                        RightSiBling->child[j] = RightSiBling->child[j + 1];
                    }

                    RightSiBling->key[RightSiBling->keynum - 1] = 0;
                    RightSiBling->child[RightSiBling->keynum - 1] = RightSiBling->child[RightSiBling->keynum];
                    RightSiBling->child[RightSiBling->keynum] = NULL;
                    --RightSiBling->keynum;

                } else {
                    for (j = child->keynum; j > 0; --j) {//元素后移
                        child->key[j] = child->key[j - 1];
                        child->child[j + 1] = child->child[j];
                    }
                    child->child[1] = child->child[0];
                    child->child[0] = LeftSiBling->child[LeftSiBling->keynum];
                    child->key[0] = node->key[index - 1];
                    ++child->keynum;

                    node->key[index - 1] = LeftSiBling->key[LeftSiBling->keynum - 1];

                    LeftSiBling->key[LeftSiBling->keynum - 1] = 0;
                    LeftSiBling->child[LeftSiBling->keynum] = NULL;

                    --LeftSiBling->keynum;
                }
            } else if ((!LeftSiBling || (LeftSiBling && LeftSiBling->keynum == BTree_Degree - 1))
                       && (!RightSiBling || (RightSiBling && RightSiBling->keynum == BTree_Degree - 1))) {
                if (LeftSiBling && LeftSiBling->keynum == BTree_Degree - 1) {
                    BTreeMergeChild(tree, node, index - 1);
                    child = LeftSiBling;
                } else if (RightSiBling && RightSiBling->keynum == BTree_Degree - 1) {
                    BTreeMergeChild(tree, node, index);
                }
            }
        }
            BTreeRecursiveRemove(&child,key);//Delete recursively
        }
}




void BTreeMove(BTree* tree,KeyType key)
{
#ifdef DEBUG_BTREE
    printf("BTree_remove:\n");
#endif

    if(*tree==NULL)
    {
        printf("BTree is NULL!\n");
        return;
    }

    BTreeRecursiveRemove(tree,key);
}

//=====================================search====================================

BTNode * BTreeRecursiveSearch(const BTree tree, KeyType key,int * pos)
{
    int i=0;
    while (i<tree->keynum && key>tree->key[i])
    {
        ++i;
    }

    if(i<tree<tree->keynum && tree->key[i]==key)
    {
        *pos=i;
        return tree;
    }

    if(tree->IsLeaf)
        return NULL;

    DiskRead(&tree);

    return BTreeRecursiveSearch(tree->child[i],key,pos);

}

BTNode*  BTreeSearch(const BTree tree, int key,int * pos)
{
#ifdef DEBUG_BTREE
    printf("B-Tree Search!");
#endif
    if(!tree)
    {
        printf("B-Tree is NULL!!!!\n");
        return NULL;
    }
    *pos=-1;
    return BTreeRecursiveSearch(tree,key,pos);
}


//===============================create===============================

void BTreeCreate(BTree* tree, const KeyType* data,int length)
{
    int i=0, pos=-1;
    assert(tree);

#ifdef DEBUG_BTREE
    printf("\n Starting create a B-Tree,the key is \n");
    for(i=0;i<length;++i)
    {
        printf("%c",data[i]);
    }
    printf("\n");
#endif

    for(i=0; i<length;i++){
#ifdef DEBUG_BTREE
        printf("\\\\\ Insert the key %c \n ",data[i]);
#endif
        BTreeSearch(*tree,data[i],&pos);
        if(pos!=-1)
            printf("This key %c is in the B-tree,not to insert.\n",data[i]);
        else
            BTreeInsert(tree,data[i]);
#ifdef DEBUG_BTREE
        BTreePrint(*tree,1);
#endif
    }
    printf("\n");
}


//===============================destroy===============================

void BTreeDestroy(BTree * tree)
{
    int i=0;
    BTNode * node =*tree;

    if(node)
    {
        for(i=0;i<node->keynum;++i)
        {
            BTreeDestroy(&node->child[i]);
        }
        free(node);
    }
    *tree=NULL;
}

