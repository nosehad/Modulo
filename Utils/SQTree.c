#include "SQTree.h"

SQTree* sqtr_create()
{
    SQTree* res = (SQTree*)malloc(sizeof(SQTree));
    res->key = null;
    res->value = null;
    res->ln = null;
    res->rn = null;
    return res;
}

SQTree* sqtr_clone(SQTree* tree)
{
    SQTree* res = (SQTree*)malloc(sizeof(SQTree));
    _sqtr_clonen(tree, res);
    return res;
}

static inline int sqtr_keyeqval(char*key1, char*key2)
{
    for(;*key1!=null; key1++, key2++)
    {
        if(*key2==null)
            return 0;
        else if(*key1 != *key2)
            return 0;
    }
    if(*key2 != null)
        return 0;
    return 1;
}

int sqtr_empty(SQTree*tree)
{
    if(tree->ln == null && tree->rn == null)
        return  1;
    return 0;
}

void _sqtr_clonen(SQNode* node1, SQNode* node2)
{
    node2->key = node1->key;
    node2->value = node1->value;

    /* dont just copy pointers, allocate new node */
    if(node1->ln != null)
    {
        node2->ln = (SQNode*)malloc(sizeof(SQNode)); /* also clonen for childs*/
        _sqtr_clonen(node1->ln, node2->ln);
    }
    if(node1->rn != null) /* do same for right child */
    { 
        node2->rn = (SQNode*)malloc(sizeof(SQNode));
        _sqtr_clonen(node1->rn, node2->rn);
    }
}

void sqtr_setNoCopy(SQTree* tree, char* key, char* value)
{
    /* no copy*/
    char*_key = key;
    /* right shift bits */
    unsigned char shifting_bits = 0;
    for(;*key != 0; shifting_bits++)
    {
        if(tree->key != null && sqtr_keyeqval(tree->key, _key) == 1)
        {
            free(tree->value);free(key);
            tree->value = value;
            return;
        }
        if(shifting_bits == 8)
        {
            shifting_bits = 0;
            key++; /* increase byte right shift*/
        }
        if(((*key) >> shifting_bits) & 1)
        {
            if(tree->rn == null) /* if right node is null -> set right node to insert node */
            {
                SQNode* insert = (SQNode*)malloc(sizeof(SQNode));
                insert->rn = null;
                insert->ln = null;
                insert->key = key;
                insert->value = value;
                tree->rn = insert;
                return;
            }
            /* not directly set tree node, to reinsert leafs of node that is beeing removed */
            tree = tree->rn; 
            continue;
        }
        if(tree->ln == null) /* if left node is null -> set left node to insert node */
        { 
            SQNode* insert = (SQNode*)malloc(sizeof(SQNode));
            insert->rn = null;
            insert->ln = null;
            insert->key = key;
            insert->value = value;
            tree->ln = insert;
            return;
        }
        tree = tree->ln;
        continue;
    }
}

void sqtr_set(SQTree* tree, char* key, char* value)
{
    /* copy value */
    sstr_createeOnStack(value_string);
    sstr_appendcs(value_string, value);
    char*_key = key;
    /* right shift bits */
    unsigned char shifting_bits = 0;
    for(;*key != 0; shifting_bits++)
    {
        if(tree->key != null && sqtr_keyeqval(tree->key, _key) == 1)
        {
            free(tree->value);
            tree->value = sstr_serialize(value_string);
            return;
        }
        if(shifting_bits == 8)
        {
            shifting_bits = 0;
            key++; /* increase byte right shift*/
        }
        if(((*key) >> shifting_bits) & 1)
        {
            if(tree->rn == null) /* if right node is null -> set right node to insert node */
            {
                SQNode* insert = (SQNode*)malloc(sizeof(SQNode));
                insert->rn = null;
                insert->ln = null;
                sstr_createeOnStack(key_string);
                sstr_appendcs(key_string, key);
                insert->key = sstr_serialize(key_string);
                insert->value = sstr_serialize(value_string);
                tree->rn = insert;
                return;
            }
            /* not directly set tree node, to reinsert leafs of node that is beeing removed */
            tree = tree->rn; 
            continue;
        }
        if(tree->ln == null) /* if left node is null -> set left node to insert node */
        { 
            SQNode* insert = (SQNode*)malloc(sizeof(SQNode));
            insert->rn = null;
            insert->ln = null;
            sstr_createeOnStack(key_string);
            sstr_appendcs(key_string, key);
            insert->key = sstr_serialize(key_string);
            insert->value = sstr_serialize(value_string);
            tree->ln = insert;
            return;
        }
        tree = tree->ln;
        continue;
    }
}

void* sqtr_get(SQTree* tree, char* key)
{
    char*_key = key;
    /* right shift bits */
    unsigned char shifting_bits = 0;
    for(;*key != 0; shifting_bits++)
    {
        if(tree->key != null && strcmp(tree->key, _key) == 0)
        {
            return tree->value;
        }
        if(shifting_bits == 8)
        {
            shifting_bits = 0;
            key++; /* increase byte right shift*/
        }
        if(((*key) >> shifting_bits) & 1)
        {
            if(tree->rn == null)
            {
                /* return if branch ends */
                return null;
            }
            /* not directly set tree node, to reinsert leafs of node that is beeing removed */
            tree = tree->rn; 
            continue;
        }
        if(tree->ln == null)
        {
            return null;
        }
        tree = tree->ln;
        continue;
    }
    /* statement not needed but required to prevent c++ warning */ return null;
}

static inline void _sqtr_insertn(SQNode* start, unsigned int startb, SQNode* insert)
{
    /* recursively insert subnodes */
    if(insert->rn != null)
    {
        _sqtr_insertn(start, startb, insert->rn); /* reinsert right branch of removed node */
        insert->rn = null;
    }
    if(insert->ln != null)
    {
        _sqtr_insertn(start, startb, insert->ln);
        insert->ln = null;
    }

    /* right shift bits */
    unsigned char shifting_bits = startb & 8;
    char*key = insert->key + (startb-shifting_bits);
    for(;*key != null; shifting_bits++)
    {
        if(shifting_bits == 8)
        {
            shifting_bits = 0;
            key++; /* increase byte right shift*/
        }
        if(((*key) >> shifting_bits) & 1)
        {
            if(start->rn == null)
            {
                start->rn = insert; /* change right node  */
                return;
            }
            start = start->rn; 
            continue;
        }
        if(start->ln == null)
        {
            start->ln = insert; /* change left node  */
            return;
        }
        start = start->ln;
        continue;
    }
}

void sqtr_delete(SQTree* tree, char* key)
{
    SQNode*next;
    char*_key = key;
    unsigned char shifting_bits = 0;
    for(;*key != null; shifting_bits++)
    {
        if(shifting_bits == 8)
        {
            shifting_bits = 0;
            key++; /* increase byte right shift*/
        }
        if(((*key) >> shifting_bits) & 1)
        {
            if(tree->rn == null)
            {
                return;
            }
            /* not directly set tree node, to reinsert leafs of node that is beeing removed */
            next = tree->rn;
            if(sqtr_keyeqval(next->key, _key) == 1)
            {
                /* set right node to null, to remove old node out of structure */
                tree->rn = null;
                /* reinsert nodes */
                int startb = (_key-key)+shifting_bits;
                if(next->rn != null)
                    _sqtr_insertn(tree, startb, next->rn); /* reinsert right branch of removed node */
                if(next->ln != null)
                    _sqtr_insertn(tree, startb, next->ln); /* reinsert left branch of removed node */
                return;
            }
            tree = next; 
            continue;
        }
        if(tree->ln == null)
        {
            return;
        }
        /* not directly set tree node, to reinsert leafs of node that is beeing removed */
        next = tree->ln;
        if(sqtr_keyeqval(next->key, _key) == 1)
        {
            /* set right node to null, to remove old node out of structure */
            tree->ln = null;
            /* reinsert nodes */
            int startb = (_key-key)+shifting_bits;
            if(next->rn != null)
                _sqtr_insertn(tree, startb, next->rn); /* reinsert right branch of removed node */
            if(next->ln != null)
                _sqtr_insertn(tree, startb, next->ln); /* reinsert left branch of removed node */
            return;
        }
        tree = next; 
        continue;
    }
}

void*sqtr_pop(SQTree* tree, char* key)
{
    SQNode*next;
    char*_key = key;
    unsigned char shifting_bits = 0;
    for(;*key != null; shifting_bits++)
    {
        if(shifting_bits == 8)
        {
            shifting_bits = 0;
            key++; /* increase byte right shift*/
        }
        if(((*key) >> shifting_bits) & 1)
        {
            if(tree->rn == null)
            {
                return null;
            }
            /* not directly set tree node, to reinsert leafs of node that is beeing removed */
            next = tree->rn;
            if(sqtr_keyeqval(next->key, _key) == 1)
            {
                /* set right node to null, to remove old node out of structure */
                tree->rn = null;
                /* reinsert nodes */
                int startb = (_key-key)+shifting_bits;
                if(next->rn != null)
                    _sqtr_insertn(tree, startb, next->rn); /* reinsert right branch of removed node */
                if(next->ln != null)
                    _sqtr_insertn(tree, startb, next->ln); /* reinsert left branch of removed node */
                void* ret = next->value;
                free(next);
                return ret;
            }
            tree = next; 
            continue;
        }
        if(tree->ln == null)
        {
            return null;
        }
        /* not directly set tree node, to reinsert leafs of node that is beeing removed */
        next = tree->ln;
        if(sqtr_keyeqval(next->key, _key) == 1)
        {
            /* set right node to null, to remove old node out of structure */
            tree->ln = null;
            /* reinsert nodes */
            int startb = (_key-key)+shifting_bits;
            if(next->rn != null)
                _sqtr_insertn(tree, startb, next->rn); /* reinsert right branch of removed node */
            if(next->ln != null)
                _sqtr_insertn(tree, startb, next->ln); /* reinsert left branch of removed node */
            void* ret = next->value;
            free(next);
            return ret;
        }
        tree = next; 
        continue;
    }
    /* statement not needed but required to prevent c++ warning */return null;
}


SQNode*sqtr_popl(SQTree* tree)
{
    SQNode*_tree = tree;
    SQNode*previous = tree;
    for(;;)
    {
        if(tree->ln != null)
        {
            previous = tree;
            tree = tree->ln;
        }
        else if(tree->rn != null)
        {
            previous = tree;
            tree = tree->rn;
        }
        else
        {
            if(previous == tree)
             {
                return null;
             }
            else 
            {
                if(previous->rn == tree)
                    previous->rn = null;
                else
                    previous->ln = null;
                return tree;
            }
        }
    }
}

void sqtr_free(SQTree *tree)
{
    if (sqtr_empty(tree))
        return;
    for (SQNode *node = sqtr_popl(tree);; node = sqtr_popl(tree))
    {
        if (node->key != null)
            free(node->key);
        if (node->value != null)
            free(node->value);
        free(node);
        if (sqtr_empty(tree))
            break;
    }
    free(tree);
}

/* not required */
void sqtr_print(SQTree*tree)
{
    /* get window size */
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    /* print rows to prevent deletion of content*/
    int i;
    for(i = 0; i < w.ws_row; ++i)
        printf("\n");
    /* ansi escape codes */
    printf("\033[2J"); /* erase screen */
    printf("\033[0;0H"); /* move cursor */
    sqtr_printn(tree, tree, w.ws_col/2, 1);
    printf("\033[%d;%dH", w.ws_row, 0);
}

void sqtr_printn(SQTree*tree, SQNode*node, int col, int row)
{
    if (node == null) {
        return; /* return at end of branch*/
    }
    /* move cursor left */
    printf("\033[%dC", col); 
    /* move cursor down */
    printf("\033[%dB", row);
    /* using SString to get lenght of string */
    sstr_createeOnStack(str); /* string representates key of current node */
    if(node->key != null)
        sstr_appendcs(str, node->key);
    else
        sstr_appendcs(str, (char*)"null");
    printf("\033[%dm", 30 + rand()%9); /* colour text */
    sstr_printf(str);
    printf("\033[0m"); /* reset color */
    printf("\033[%dD", str->s_size); /* move cursor back by sizeof string */
    printf("\033[1B");
    sstr_clear(str);
    /* print "leafes" */
    if(node->ln != null)
        sstr_appendc(str, '/');
    if(node->rn != null)
        sstr_appendc(str, '\\');
    sstr_printf(str);
    /* move cursor back */
    printf("\033[%dD", str->s_size);
    printf("\033[%dD", col);
    printf("\033[%dA", row+1);
    /* print subnodes*/
    sqtr_printn(tree, node->rn, col + 2, row + 2); /* change col for tree structure */
    sqtr_printn(tree, node->ln, col - 3, row + 2);
}

void sqtr_printbin(SQTree*tree)
{
    /* get window size */
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    /* print rows to prevent deletion of content*/
    int i;
    for(i = 0; i < w.ws_row; ++i)
        printf("\n");
    /* ansi escape codes */
    printf("\033[2J"); /* erase screen */
    printf("\033[0;0H"); /* move cursor */
    sqtr_printnbin(tree, tree, w.ws_col/2, 1, 0);
    printf("\033[%d;%dH", w.ws_row, 0);
}

void sqtr_printnbin(SQTree*tree, SQNode*node, int col, int row, char val)
{
    if (node == null) {
        return; /* return at end of branch*/
    }
    /* move cursor left */
    printf("\033[%dC", col); 
    /* move cursor down */
    printf("\033[%dB", row);
    /* using SString to get lenght of string */
    sstr_createeOnStack(str); /* string representates key of current node */
    sstr_appendd(str, val);
    printf("\033[%dm", 30 + rand()%9); /* colour text */
    sstr_printf(str);
    printf("\033[0m"); /* reset color */
    printf("\033[%dD", str->s_size); /* move cursor back by sizeof string */
    printf("\033[1B");
    sstr_clear(str);
    /* print "leafes" */
    if(node->ln != null)
        sstr_appendc(str, '/');
    if(node->rn != null)
        sstr_appendc(str, '\\');
    sstr_printf(str);
    /* move cursor back */
    printf("\033[%dD", str->s_size);
    printf("\033[%dD", col);
    printf("\033[%dA", row+1);
    /* print subnodes*/
    sqtr_printnbin(tree, node->rn, col + 1, row + 2, 1); /* change col for tree structure */
    sqtr_printnbin(tree, node->ln, col - 2, row + 2, 0);
}