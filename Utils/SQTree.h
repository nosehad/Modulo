#ifndef SQTREE_H /* just little different to bs tree */
    #define SQTREE_H

    #ifdef __cplusplus
        extern "C" {
    #endif

    #include <sys/ioctl.h>

    #include <stdlib.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <string.h>

    #include "SString.h"
    #include "null.h"


    typedef struct _qNode SQNode;
    #define SQTree struct _qNode  /* Root node */
    #define TreeMap(key_dt, value_dt) struct _qNode  /* Root node */
    typedef struct _qIterator SQIterator;

    /* creates new tree */
    SQTree* sqtr_create();
    /* check if one key is equal to another*/
    //STATIC_I int sqtr_keyeqval(char*key1, char*key2);
    /* check if tree is empty */
    int sqtr_empty(SQTree*tree);
    /* recursively clone Tree */
    SQTree* sqtr_clone(SQTree* tree);
    void _sqtr_clonen(SQNode* node1, SQNode* node2);
    /* sets value of <key> to <value> */
    /* no copy */
    /* creates new node if there is no node with <key> */
    void sqtr_setNoCopy(SQTree* tree, char* key, char* value);
    /* sets value of <key> to <value> */
    /* copies values of <key> and <value> into an new buffer */
    /* creates new node if there is no node with <key> */
    void sqtr_set(SQTree* tree, char* key, char* value);
    /* returns element that belongs to <key> */
    void* sqtr_get(SQTree* tree, char* key);
    #define sqtr_contains(tree, key) sqtr_get(tree, key) == null;
    /* completely deletes node out of tree, also cleans up tree structure, very slow, since every subnode has to be reinserted */
    void sqtr_delete(SQTree* tree, char* key);
    /* marks node with <key> as free returns value of node that belongs to <key> */
    void*sqtr_pop(SQTree* tree, char* key);
    /* pop leftmost element of tree */
    SQNode*sqtr_popl(SQTree* tree);
    /* completely deletes whole tree */
    void sqtr_free(SQTree *tree);
    /* used to insert node at specific point */
    //STATIC_I void _sqtr_insertn(SQNode* start, unsigned int startb, SQNode* insert);
    /* prints entire tree */
    void sqtr_print(SQTree*tree);
    /* prints node at row|col*/
    void sqtr_printn(SQTree*tree, SQNode*node, int col, int row);
    /* print tree in "binary" format */
    void sqtr_printbin(SQTree*tree);
    void sqtr_printnbin(SQTree*tree, SQNode*node, int col, int row, char val);

    struct _qNode
    {
        char* key;
        char* value;
        struct _qNode* ln; /* left node */
        struct _qNode* rn; /* right node */
    };

    #ifdef __cplusplus
        }
    #endif
#endif 