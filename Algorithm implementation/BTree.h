/***************************************************************************
    @author: Zizhao Wang
    @date:   2021/6/11
    @IDE:    Clion 2021.1
**********************************************************/

#ifndef ALGORITHM_IMPLEMENTATION_BTREE_H
#define ALGORITHM_IMPLEMENTATION_BTREE_H

#ifndef BTREE_H
#define BTREE_H

#ifdef __cplusplus
extern "C"
{
#endif
#define BTree_Degree    2
#define BTree_Order     (BTree_Degree * 2) // The order is the maximum number of words that the internal node can take, which is twice the degree.

    typedef int KeyType;
    typedef struct BTNode
    {
        int keynum;
        KeyType key[BTree_Order-1];
        struct BTNode * child[BTree_Order];
        char IsLeaf;
    }BTNode;

    typedef BTNode* BTree; // Define a B-Tree.

    //Create a B-Tree
    void BTreeCreate(BTree* tree, const KeyType* data,int length);

    //Delete a B-Tree
    void BTreeDestroy(BTree * tree);

    // Insert a key
    void BTreeInsert(BTree* tree, KeyType key);

    // Delete a Key in B-Tree
    void BTreeMove(BTree* tree,KeyType key);

    // Print each layer of information
    void BTreePrint(const BTree tree,int layer);

    //Find the key in the B-Tree.If success,return address and position.
    BTNode*  BTreeSearch(const BTree tree, int key,int * pos);
#ifdef __cplusplus
};
#endif

#endif //ALGORITHM_IMPLEMENTATION_BTREE_H
