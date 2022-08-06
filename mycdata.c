#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "mycdata.h"

/*
 * ---------------------------------------------- Print Message
 */

void printError(const char *msg, ...)
{
    printf("Error: ");
    if (msg)
    {
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    }
}
void printWarn(const char *msg, ...)
{
    printf("Warn: ");
    if (msg)
    {
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    }
}
void printInfo(const char *msg, ...)
{
    printf("Info: ");
    if (msg)
    {
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    }
}
void printDebug(const char *msg, ...)
{
    printf("Debug: ");
    if (msg)
    {
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    }
}

/*
 * ---------------------------------------------- Stack
 */

struct Stack *stackNew()
{
    struct Stack *p = malloc(sizeof(struct Stack));
    if (p == NULL)
        return NULL;
    p->els = NULL;
    p->cap = 8;
    p->size = 0;
    return p;
}
void stackFree(struct Stack *p)
{
    if (p == NULL)
        return;
    if (p->els != NULL)
        free(p->els);
    free(p);
}
int stackPush(struct Stack *p, void *el)
{
    if (p == NULL)
        return 0;
    if (p->els == NULL)
    {
        p->els = malloc(sizeof(void *) * p->cap);
        if (p->els == NULL)
            return 0;
    }
    if (p->size == p->cap)
    {
        void **q = realloc(p->els, sizeof(void *) * p->cap << 1);
        if (q == NULL)
            return 0;
        p->els = q;
        p->cap = p->cap << 1;
    }
    *(p->els + p->size) = el;
    p->size++;
    return 1;
}
void *stackPop(struct Stack *p)
{
    if (p == NULL)
        return NULL;
    if (p->size == 0)
        return NULL;
    void *el = *(p->els + p->size - 1);
    p->size--;
    return el;
}
void *stackPeek(struct Stack *p)
{
    return p == NULL || p->size == 0 ? NULL : *(p->els + p->size - 1);
}
int stackSize(struct Stack *p)
{
    return p == NULL ? 0 : p->size;
}
void stackClear(struct Stack *p)
{
    p->size = 0;
}

/*
 * ---------------------------------------------- Queue
 */

struct Queue *queueNew()
{
    struct Queue *p = malloc(sizeof(struct Queue));
    if (p == NULL)
        return NULL;
    p->els = NULL;
    p->cap = 8;
    p->size = p->head = p->tail = 0;
    return p;
}
void queueFree(struct Queue *p)
{
    if (p == NULL)
        return;
    if (p->els != NULL)
        free(p->els);
    free(p);
}
int queueOffer(struct Queue *p, void *el)
{
    if (p == NULL)
        return 0;
    if (p->els == NULL)
    {
        p->els = malloc(sizeof(void *) * p->cap);
        if (p->els == NULL)
            return 0;
    }
    if (p->size == p->cap)
    {
        void **q = realloc(p->els, sizeof(void *) * p->cap << 1);
        if (q == NULL)
            return 0;
        p->els = q;
        p->cap = p->cap << 1;
        if (p->tail < p->head)
        {
            int i;
            for (i = 0; i <= p->tail; i++)
                *(p->els + (p->cap >> 1) + i) = *(p->els + i);
            p->tail = p->head + p->size - 1;
        }
    }
    int i;
    if (p->size == 0)
        p->head = i = 0;
    else
    {
        i = p->tail + 1;
        if (i == p->cap)
            i = 0;
    }
    *(p->els + i) = el;
    p->tail = i;
    p->size++;
    return 1;
}
void *queuePoll(struct Queue *p)
{
    if (p == NULL)
        return NULL;
    if (p->size == 0)
        return NULL;
    void *el = *(p->els + p->head);
    p->size--;
    if (p->size == 0)
        p->head = p->tail = 0;
    else
    {
        p->head++;
        if (p->head == p->cap)
            p->head = 0;
    }
    return el;
}
void *queuePeek(struct Queue *p)
{
    return p == NULL || p->size == 0 ? NULL : *(p->els + p->head);
}
int queueSize(struct Queue *p)
{
    return p == NULL ? 0 : p->size;
}
void queueClear(struct Queue *p)
{
    p->size = p->head = p->tail = 0;
}

/*
 * ---------------------------------------------- Avl Tree
 */

static struct avlTreeNode *avlTreeNodeNew(int k, void *v);
static void avlTreeNodeFree(struct avlTreeNode *p);
static void treeRebalance(struct avlTreeNode *b, struct avlTreeNode *n);
static struct avlTreeNode *avlTreeRotateLeft(struct avlTreeNode *n);
static struct avlTreeNode *avlTreeRotateRight(struct avlTreeNode *n);
static void reheight(struct avlTreeNode *n);
static int lh(struct avlTreeNode *n);
static int rh(struct avlTreeNode *n);
static int max(int a, int b);

struct avlTree *avlTreeNew(int (*key)(void *))
{
    if (!key)
        return NULL;
    struct avlTree *p = malloc(sizeof(struct avlTree));
    if (!p)
        return NULL;
    p->root = NULL;
    p->size = 0;
    p->key = key;
    return p;
}
void avlTreeFree(struct avlTree *p)
{
    if (!p)
        return;
    if (p->root)
    {
        struct Stack *s = stackNew();
        stackPush(s, p->root);
        while ((struct avlTreeNode *)stackPeek(s))
        {
            struct avlTreeNode *n = (struct avlTreeNode *)stackPop(s);
            if (n->right)
                stackPush(s, n->right);
            if (n->left)
                stackPush(s, n->left);
            avlTreeNodeFree(n);
        }
        stackFree(s);
    }
    free(p);
}
static struct avlTreeNode *avlTreeNodeNew(int k, void *v)
{
    struct avlTreeNode *p = malloc(sizeof(struct avlTreeNode));
    if (!p)
        return NULL;
    p->parent = p->left = p->right = NULL;
    p->height = 1;
    p->key = k;
    p->val = v;
    return p;
}
static void avlTreeNodeFree(struct avlTreeNode *p)
{
    if (p)
        free(p);
}
static void treeRebalance(struct avlTreeNode *b, struct avlTreeNode *n)
{
    if (b)
    {
        if (b->key > n->key)
        {
            /* left */
            if (b->left->key > n->key)
            {
                /* left left */
                avlTreeRotateRight(b);
            }
            else
            {
                /* left right */
                avlTreeRotateLeft(b->left);
                avlTreeRotateRight(b);
            }
        }
        else
        {
            /* right */
            if (b->right->key > n->key)
            {
                /* right left */
                avlTreeRotateRight(b->right);
                avlTreeRotateLeft(b);
            }
            else
            {
                /* right right */
                avlTreeRotateLeft(b);
            }
        }
    }
}
static struct avlTreeNode *avlTreeRotateLeft(struct avlTreeNode *n)
{
    struct avlTreeNode *p = n->parent;
    struct avlTreeNode *r = n->right;

    if (p->left == n)
        p->left = r;
    else
        p->right = r;

    n->parent = r;
    n->right = r->left;
    n->height = max(lh(n->left), rh(n->right)) + 1;

    r->parent = p;
    r->left = n;
    r->height = max(lh(r->left), rh(r->right)) + 1;

    return r;
}
static struct avlTreeNode *avlTreeRotateRight(struct avlTreeNode *n)
{
    struct avlTreeNode *p = n->parent;
    struct avlTreeNode *l = n->left;

    if (p->left == n)
        p->left = l;
    else
        p->right = l;

    n->parent = l;
    n->left = l->right;
    n->height = max(lh(n->left), rh(n->right)) + 1;

    l->parent = p;
    l->right = n;
    l->height = max(lh(l->left), rh(l->right)) + 1;

    return l;
}
static int lh(struct avlTreeNode *n)
{
    return n && n->left ? n->left->height : 0;
}
static int rh(struct avlTreeNode *n)
{
    return n && n->right ? n->right->height : 0;
}
static int max(int a, int b)
{
    return a > b ? a : b;
}
static void reheight(struct avlTreeNode *n)
{
    while (n)
    {
        n->height = max(lh(n), rh(n)) + 1;
        n = n->parent;
    }
}
int avlTreeAdd(struct avlTree *p, void *el)
{
    if (!p)
        return 0;

    struct avlTreeNode *n;
    if (n = avlTreeSearch(p, el))
    {
        n->val = el;
        return 1;
    }

    int k = (*p->key)(el);
    n = avlTreeNodeNew(k, el);
    if (!n)
        return 0;

    if (p->root == NULL)
    {
        p->root = n;
        p->size++;
        return 1;
    }

    struct avlTreeNode *q = p->root;
    struct avlTreeNode *b;
    while (q)
    {
        if (q->key > k)
        {
            /* left */
            if (abs(lh(q) + 1 - rh(q)) > 1)
                b = q;

            if (q->left)
                q = q->left;
            else
            {
                q->left = n;
                n->parent = q;
                p->size++;
                if (b)
                    treeRebalance(b, n);
                else
                    reheight(n);
                break;
            }
        }
        else
        {
            /* right */
            if (abs(lh(q) - (rh(q) + 1)) > 1)
                b = q;

            if (q->right)
                q = q->right;
            else
            {
                q->right = n;
                n->parent = q;
                p->size++;
                if (b)
                    treeRebalance(b, n);
                else
                    reheight(n);
                break;
            }
        }
    }
    return 1;
}
int avlTreeRemove(struct avlTree *p, void *el)
{
    if (!p)
        return 0;
    struct avlTreeNode *n;
    if (!(n = avlTreeSearch(p, el)))
        return 0;
    return 0;
}
void *avlTreeSearch(struct avlTree *p, void *el)
{
    if (!p)
        return NULL;
    int k = (*p->key)(el);
    struct avlTreeNode *n = p->root;
    while (n)
    {
        if (n->key == k)
            return n->val;
        n = n->key > k ? n->left : n->right;
    }
    return NULL;
}
void avlTreePrint(struct avlTree *p, void (*printVal)(void *))
{
    if (p && p->root && printVal)
    {
        struct Stack *stack = stackNew();
        if (!stack)
            return;

        stackPush(stack, p->root);
        struct avlTreeNode *prev, *curr;
        while (stackPeek(stack))
        {
            curr = (struct avlTreeNode *)stackPop(stack);
            if (curr->left && prev != curr->left)
            {
                stackPush(stack, curr);
                stackPush(stack, curr->left);
            }
            else
            {
                printVal(curr->val);
                prev = curr;
                if (curr->right)
                    stackPush(stack, curr->right);
            }
        }
    }
}
