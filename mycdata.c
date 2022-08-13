#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "mycdata.h"

/*
 * ---------------------------------------------- Common
 */

static int max(int a, int b)
{
    return a > b ? a : b;
}

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
static struct avlTreeNode *avlTreeBalance(struct avlTreeNode *b);
static struct avlTreeNode *avlTreeRotateLeft(struct avlTreeNode *n);
static struct avlTreeNode *avlTreeRotateRight(struct avlTreeNode *n);
static int avlTreeLh(struct avlTreeNode *n);
static int avlTreeRh(struct avlTreeNode *n);
static struct avlTreeNode *avlTreeFindMin(struct avlTreeNode *n);

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
    p->height = 0;
    p->key = k;
    p->val = v;
    return p;
}
static void avlTreeNodeFree(struct avlTreeNode *p)
{
    if (p)
        free(p);
}
int avlTreeAdd(struct avlTree *p, void *el)
{
    if (!p)
    {
        printError("avlTree p is NULL\n");
        return 0;
    }

    int k = (*p->key)(el);

    struct avlTreeNode *q = p->root;
    while (q)
    {
        if (q->key > k)
        {
            /* left */
            if (q->left)
                q = q->left;
            else
                break;
        }
        else if (q->key < k)
        {
            if (q->right)
                q = q->right;
            else
                break;
        }
        else
        {
            q->val = el;
            return 1;
        }
    }

    struct avlTreeNode *n = avlTreeNodeNew(k, el);
    if (!n)
        return 0;

    if (q)
    {
        n->parent = q;
        if (q->key > k)
            q->left = n;
        else
            q->right = n;
    }

    struct avlTreeNode *b = n;
    while (b)
    {
        struct avlTreeNode *r = avlTreeBalance(b);
        b = r->parent;
        if (!b)
        {
            p->root = r;
            break;
        }
    }

    p->size++;

    return 1;
}
int avlTreeRemove(struct avlTree *p, void *el)
{
    if (!p)
    {
        printError("avlTree p is NULL\n");
        return 0;
    }

    struct avlTreeNode *n = avlTreeSearch(p, el);
    if (!n)
    {
        printDebug("avlTreeNode not found\n");
        return 0;
    }

    if (n->left && n->right)
    {
        struct avlTreeNode *rm = avlTreeFindMin(n->right);
        n->key = rm->key;
        n->val = rm->val;

        if (rm->parent->left == rm)
            rm->parent->left = rm->right;
        else
            rm->parent->right = rm->right;
        if (rm->right)
            rm->right->parent = rm->parent;

        struct avlTreeNode *b = rm->parent;
        avlTreeNodeFree(rm);
        while (b)
        {
            b = avlTreeBalance(b);
            if (b->parent)
                b = b->parent;
            else
            {
                p->root = b;
                break;
            }
        }
    }
    else if (n->left)
    {
        struct avlTreeNode *l = n->left;
        n->key = l->key;
        n->val = l->val;

        n->left = l->left;
        if (n->left)
            n->left->parent = n;

        n->right = l->right;
        if (n->right)
            n->right->parent = n;

        avlTreeNodeFree(l);

        while (n)
        {
            n = avlTreeBalance(n);
            if (n->parent)
                n = n->parent;
            else
            {
                p->root = n;
                break;
            }
        }
    }
    else if (n->right)
    {
        struct avlTreeNode *r = n->right;
        n->key = r->key;
        n->val = r->val;

        n->left = r->left;
        if (n->left)
            n->left->parent = n;

        n->right = r->right;
        if (n->right)
            n->right->parent = n;

        avlTreeNodeFree(r);

        while (n)
        {
            n = avlTreeBalance(n);
            if (n->parent)
                n = n->parent;
            else
            {
                p->root = n;
                break;
            }
        }
    }
    else
    {
        if (n->parent)
        {
            if (n == n->parent->left)
                n->parent->left = NULL;
            else
                n->parent->right = NULL;

            struct avlTreeNode *b = n->parent;
            while (b)
            {
                b = avlTreeBalance(b);
                if (b->parent)
                    b = b->parent;
                else
                {
                    p->root = b;
                    break;
                }
            }
        }
        else
            p->root = NULL;
        avlTreeNodeFree(n);
    }

    p->size--;

    return 1;
}
void *avlTreeSearch(struct avlTree *p, void *el)
{
    if (p && p->root)
    {
        int k = (*p->key)(el);
        struct avlTreeNode *n = p->root;
        while (n)
        {
            if (n->key == k)
                return n;
            n = n->key > k ? n->left : n->right;
        }
    }
    return NULL;
}
static struct avlTreeNode *avlTreeBalance(struct avlTreeNode *b)
{
    if (avlTreeLh(b) - avlTreeRh(b) > 1)
    {
        /* left */
        if (avlTreeLh(b->left) >= avlTreeRh(b->left))
        {
            /* left left */
            return avlTreeRotateRight(b);
        }
        else
        {
            /* left right */
            avlTreeRotateLeft(b->left);
            return avlTreeRotateRight(b);
        }
    }
    else if (avlTreeRh(b) - avlTreeLh(b) > 1)
    {
        /* right */
        if (avlTreeRh(b->right) >= avlTreeLh(b->right))
        {
            /* right right */
            return avlTreeRotateLeft(b);
        }
        else
        {
            /* right left */
            avlTreeRotateRight(b->right);
            return avlTreeRotateLeft(b);
        }
    }
    else
    {
        b->height = max(avlTreeLh(b), avlTreeRh(b)) + 1;
        return b;
    }
}
static struct avlTreeNode *avlTreeRotateLeft(struct avlTreeNode *n)
{
    struct avlTreeNode *p = n->parent;
    struct avlTreeNode *r = n->right;

    if (p)
    {
        if (p->left && p->left == n)
            p->left = r;
        else
            p->right = r;
    }

    n->parent = r;
    n->right = r->left;
    if (n->right)
        n->right->parent = n;
    n->height = max(avlTreeLh(n), avlTreeRh(n)) + 1;

    r->parent = p;
    r->left = n;
    r->height = max(avlTreeLh(r), avlTreeRh(r)) + 1;

    return r;
}
static struct avlTreeNode *avlTreeRotateRight(struct avlTreeNode *n)
{
    struct avlTreeNode *p = n->parent;
    struct avlTreeNode *l = n->left;

    if (p)
    {
        if (p->left && p->left == n)
            p->left = l;
        else
            p->right = l;
    }

    n->parent = l;
    n->left = l->right;
    if (n->left)
        n->left->parent = n;
    n->height = max(avlTreeLh(n), avlTreeRh(n)) + 1;

    l->parent = p;
    l->right = n;
    l->height = max(avlTreeLh(l), avlTreeRh(l)) + 1;

    return l;
}
static int avlTreeLh(struct avlTreeNode *n)
{
    return n->left ? n->left->height : -1;
}
static int avlTreeRh(struct avlTreeNode *n)
{
    return n->right ? n->right->height : -1;
}
static struct avlTreeNode *avlTreeFindMin(struct avlTreeNode *n)
{
    while (n->left)
        n = n->left;
    return n;
}
#ifdef DEBUG
static int avlTreeHeight(struct avlTreeNode *n)
{
    if (n)
        return max(avlTreeHeight(n->left), avlTreeHeight(n->right)) + 1;
    else
        return -1;
}
static void avlTreeCheck(struct avlTreeNode *n)
{
    if (n)
    {
        if (n->left && n->left->parent != n)
            printError("parent set error");
        if (n->right && n->right->parent != n)
            printError("parent set error");

        if (avlTreeHeight(n->left) - avlTreeHeight(n->right) > 1)
            printError("error");
        if (avlTreeHeight(n->right) - avlTreeHeight(n->left) > 1)
            printError("error");
    }
}
void avlTreePrint(struct avlTree *p, void (*printVal)(void *))
{
    if (p && p->root && printVal)
    {
        struct Stack *stack = stackNew();
        if (!stack)
            return;

        stackPush(stack, p->root);
        struct avlTreeNode *printed = NULL;
        while (stackPeek(stack))
        {
            struct avlTreeNode *n = (struct avlTreeNode *)stackPop(stack);
            avlTreeCheck(n);
            if (n->left)
            {
                if (printed && printed->key >= n->left->key)
                {
                    printVal(n->val);
                    printed = n;
                    if (n->right)
                        stackPush(stack, n->right);
                }
                else
                {
                    stackPush(stack, n);
                    stackPush(stack, n->left);
                }
            }
            else
            {
                printVal(n->val);
                printed = n;
                if (n->right)
                    stackPush(stack, n->right);
            }
        }

        stackFree(stack);
    }
}
#endif
