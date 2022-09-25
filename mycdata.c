#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>
#include "mycdata.h"

/*
 * ---------------------------------------------------------------------- Common
 */

static int max(int a, int b)
{
    return a > b ? a : b;
}

/*
 * --------------------------------------------------------------- Print Message
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
 * ----------------------------------------------------------------------- Stack
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
 * ----------------------------------------------------------------------- Queue
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
 * -------------------------------------------------------------------- Avl Tree
 */

static struct avlTreeNode *avlTreeNodeNew(int k, void *v);
static void avlTreeNodeFree(struct avlTreeNode *p);
static struct avlTreeNode *avlTreeBalance(struct avlTreeNode *b);
static struct avlTreeNode *avlTreeRotateLeft(struct avlTreeNode *n);
static struct avlTreeNode *avlTreeRotateRight(struct avlTreeNode *n);
static int avlTreeLh(struct avlTreeNode *n);
static int avlTreeRh(struct avlTreeNode *n);
static struct avlTreeNode *avlTreeNodeFindMin(struct avlTreeNode *n);

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
        struct avlTreeNode *rm = avlTreeNodeFindMin(n->right);
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
    else if (n->left || n->right)
    {
        struct avlTreeNode *lr = n->left ? n->left : n->right;
        n->key = lr->key;
        n->val = lr->val;

        n->left = lr->left;
        if (n->left)
            n->left->parent = n;

        n->right = lr->right;
        if (n->right)
            n->right->parent = n;

        avlTreeNodeFree(lr);

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
void *avlTreeFindMin(struct avlTree *p)
{
    if (p && p->root)
    {
        struct avlTreeNode *n = p->root;
        while (n)
        {
            if (n->left)
                n = n->left;
            else
                return n->val;
        }
        return NULL;
    }
    return NULL;
}
void *avlTreeFindMax(struct avlTree *p)
{
    if (p && p->root)
    {
        struct avlTreeNode *n = p->root;
        while (n)
        {
            if (n->right)
                n = n->right;
            else
                return n->val;
        }
        return NULL;
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
static struct avlTreeNode *avlTreeNodeFindMin(struct avlTreeNode *n)
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

        int lh = avlTreeHeight(n->left);
        int rh = avlTreeHeight(n->right);
        if (lh - rh > 1)
            printError("error");
        if (rh - lh > 1)
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

/*
 * -------------------------------------------------------------- Red-Black Tree
 */

static struct rbTreeNode *rbTreeNodeNew(int k, void *v);
static void rbTreeNodeFree(struct rbTreeNode *p);
static struct rbTreeNode *p(struct rbTreeNode *n);
static struct rbTreeNode *l(struct rbTreeNode *n);
static struct rbTreeNode *r(struct rbTreeNode *n);
static int c(struct rbTreeNode *n);
static void rbTreeInsertFixup(struct rbTree *t, struct rbTreeNode *z);
static void rbTreeDeleteFixup(struct rbTree *t, struct rbTreeNode *x);
static void rbTreeLeftRotate(struct rbTree *t, struct rbTreeNode *x);
static void rbTreeRightRotate(struct rbTree *t, struct rbTreeNode *x);
static void rbTreeTransplant(struct rbTree *t, struct rbTreeNode *u, struct rbTreeNode *v);
static struct rbTreeNode *rbTreeNodeFindMin(struct rbTreeNode *n);

struct rbTree *rbTreeNew(int (*key)(void *))
{
    if (!key)
    {
        printError("rbTreeNew key is NULL\n");
        return NULL;
    }
    struct rbTree *p = malloc(sizeof(struct rbTree));
    if (p)
    {
        // p->root = NULL;
        p->root = RB_NIL;
        p->size = 0;
        p->key = key;
        return p;
    }
    else
    {
        printError("rbTreeNew error");
        return NULL;
    }
}
void rbTreeFree(struct rbTree *p)
{
    if (!p)
        return;
    if (p->root && p->root != RB_NIL)
    {
        struct Stack *s = stackNew();
        stackPush(s, p->root);
        while ((struct rbTreeNode *)stackPeek(s))
        {
            struct rbTreeNode *n = (struct rbTreeNode *)stackPop(s);
            if (n->right && n->right != RB_NIL)
                stackPush(s, n->right);
            if (n->left && n->left != RB_NIL)
                stackPush(s, n->left);
            rbTreeNodeFree(n);
        }
        stackFree(s);
    }
    free(p);
}
static struct rbTreeNode *rbTreeNodeNew(int k, void *v)
{
    struct rbTreeNode *p = malloc(sizeof(struct rbTreeNode));
    if (!p)
        return NULL;
    p->parent = NULL;
    p->left = p->right = RB_NIL;
    p->color = RB_RED;
    p->key = k;
    p->val = v;
    return p;
}
static void rbTreeNodeFree(struct rbTreeNode *p)
{
    if (p)
        free(p);
}
int rbTreeInsert(struct rbTree *t, void *el)
{
    if (!t)
    {
        printError("rbTreeInsert t is NULL");
        return 0;
    }

    int k = t->key(el);

    struct rbTreeNode *y = RB_NIL;
    struct rbTreeNode *x = t->root;
    while (x != RB_NIL)
    {
        y = x;
        if (k < x->key)
            x = l(x);
        else if (k > x->key)
            x = r(x);
        else
        {
            x->val = el;
            return 1;
        }
    }

    struct rbTreeNode *z = rbTreeNodeNew(k, el);
    if (!z)
    {
        printError("rbTreeNodeNew error\n");
        return 0;
    }

    z->parent = y;
    if (y == RB_NIL)
        t->root = z;
    else if (z->key < y->key)
        y->left = z;
    else
        y->right = z;

    rbTreeInsertFixup(t, z);

    t->size++;

    return 1;
}
int rbTreeDelete(struct rbTree *t, void *el)
{
    if (!t)
    {
        printError("rbTreeDelete t is NULL");
        return 0;
    }

    struct rbTreeNode *x = NULL;
    struct rbTreeNode *y = NULL;
    struct rbTreeNode *z = rbTreeSearch(t, el);

    if (!z)
        return 0;

    y = z;
    int yOriginalColor = c(y);
    if (l(z) == RB_NIL)
    {
        x = r(z);
        rbTreeTransplant(t, z, r(z));
    }
    else if (r(z) == RB_NIL)
    {
        x = l(z);
        rbTreeTransplant(t, z, l(z));
    }
    else
    {
        y = rbTreeNodeFindMin(r(z));
        yOriginalColor = c(y);
        x = r(y);
        if (p(y) == z)
            x->parent = y;
        else
        {
            rbTreeTransplant(t, y, r(y));
            y->right = r(z);
            r(y)->parent = y;
        }
        rbTreeTransplant(t, z, y);
        y->left = l(z);
        l(y)->parent = y;
        y->color = c(z);
    }
    if (yOriginalColor == RB_BLACK)
        rbTreeDeleteFixup(t, x);

    rbTreeNodeFree(z);
    t->size--;

    return 1;
}
void *rbTreeSearch(struct rbTree *p, void *el)
{
    if (p && p->root)
    {
        int k = (*p->key)(el);
        struct rbTreeNode *n = p->root;
        while (n)
        {
            if (n->key == k)
                return n;
            n = n->key > k ? n->left : n->right;
        }
    }
    return NULL;
}
void *rbTreeFindMin(struct rbTree *p)
{
    if (p && p->root && p->root != RB_NIL)
    {
        struct rbTreeNode *n = p->root;
        while (n && n != RB_NIL)
        {
            if (l(n) && l(n) != RB_NIL)
                n = l(n);
            else
                return n->val;
        }
        return NULL;
    }
    return NULL;
}
void *rbTreeFindMax(struct rbTree *p)
{
    if (p && p->root && p->root != RB_NIL)
    {
        struct rbTreeNode *n = p->root;
        while (n && n != RB_NIL)
        {
            if (r(n) && r(n) != RB_NIL)
                n = r(n);
            else
                return n->val;
        }
        return NULL;
    }
    return NULL;
}
static struct rbTreeNode *p(struct rbTreeNode *n)
{
    return n && n->parent ? n->parent : NULL;
}
static struct rbTreeNode *l(struct rbTreeNode *n)
{
    return n ? n->left : NULL;
}
static struct rbTreeNode *r(struct rbTreeNode *n)
{
    return n ? n->right : NULL;
}
static int c(struct rbTreeNode *n)
{
    return n ? n->color : RB_BLACK;
}
static void rbTreeInsertFixup(struct rbTree *t, struct rbTreeNode *z)
{
    struct rbTreeNode *y = NULL;
    while (c(p(z)) == RB_RED)
    {
        if (p(z) == l(p(p(z))))
        {
            y = r(p(p(z)));
            if (c(y) == RB_RED)
            {
                p(z)->color = RB_BLACK;  // case 1
                y->color = RB_BLACK;     // case 1
                p(p(z))->color = RB_RED; // case 1
                z = p(p(z));             // case 1
            }
            else
            {
                if (z == r(p(z)))
                {
                    z = p(z);               // case 2
                    rbTreeLeftRotate(t, z); // case 2
                }
                p(z)->color = RB_BLACK;        // case 3
                p(p(z))->color = RB_RED;       // case 3
                rbTreeRightRotate(t, p(p(z))); // case 3
            }
        }
        else
        {
            y = l(p(p(z)));
            if (c(y) == RB_RED)
            {
                p(z)->color = RB_BLACK;  // case 1
                y->color = RB_BLACK;     // case 1
                p(p(z))->color = RB_RED; // case 1
                z = p(p(z));             // case 1
            }
            else
            {
                if (z == l(p(z)))
                {
                    z = p(z);                // case 2
                    rbTreeRightRotate(t, z); // case 2
                }
                p(z)->color = RB_BLACK;       // case 3
                p(p(z))->color = RB_RED;      // case 3
                rbTreeLeftRotate(t, p(p(z))); // case 3
            }
        }
    }
    t->root->color = RB_BLACK;
}
static void rbTreeDeleteFixup(struct rbTree *t, struct rbTreeNode *x)
{
    while (x != t->root && c(x) == RB_BLACK)
    {
        if (x == l(p(x)))
        {
            struct rbTreeNode *w = r(p(x));
            if (c(w) == RB_RED)
            {
                w->color = RB_BLACK;       // case 1
                p(x)->color = RB_RED;      // case 1
                rbTreeLeftRotate(t, p(x)); // case 1
                w = r(p(x));               // case 1
            }
            if (c(l(w)) == RB_BLACK && c(r(w)) == RB_BLACK)
            {
                if (w)                 // xxxx
                    w->color = RB_RED; // case 2
                x = p(x);              // case 2
            }
            else
            {
                if (c(r(w)) == RB_BLACK)
                {
                    l(w)->color = RB_BLACK;  // case 3
                    w->color = RB_RED;       // case 3
                    rbTreeRightRotate(t, w); // case 3
                    w = r(p(x));             // case 3
                }
                w->color = c(p(x));        // case 4
                p(x)->color = RB_BLACK;    // case 4
                r(w)->color = RB_BLACK;    // case 4
                rbTreeLeftRotate(t, p(x)); // case 4
                x = t->root;               // case 4
            }
        }
        else
        {
            struct rbTreeNode *w = l(p(x));
            if (c(w) == RB_RED)
            {
                w->color = RB_BLACK;        // case 1
                p(x)->color = RB_RED;       // case 1
                rbTreeRightRotate(t, p(x)); // case 1
                w = l(p(x));                // case 1
            }
            if (c(r(w)) == RB_BLACK && c(l(w)) == RB_BLACK)
            {
                w->color = RB_RED; // case 2
                x = p(x);          // case 2
            }
            else
            {
                if (c(l(w)) == RB_BLACK)
                {
                    r(w)->color = RB_BLACK; // case 3
                    w->color = RB_RED;      // case 3
                    rbTreeLeftRotate(t, w); // case 3
                    w = l(p(x));            // case 3
                }
                w->color = c(p(x));         // case 4
                p(x)->color = RB_BLACK;     // case 4
                l(w)->color = RB_BLACK;     // case 4
                rbTreeRightRotate(t, p(x)); // case 4
                x = t->root;                // case 4
            }
        }
    }
    x->color = RB_BLACK;
}
static void rbTreeLeftRotate(struct rbTree *t, struct rbTreeNode *x)
{
    struct rbTreeNode *y = r(x);
    x->right = l(y);
    if (l(y) != RB_NIL)
        l(y)->parent = x;
    y->parent = p(x);
    if (p(x) == RB_NIL)
        t->root = y;
    else if (x == l(p(x)))
        p(x)->left = y;
    else
        p(x)->right = y;
    y->left = x;
    x->parent = y;
}
static void rbTreeRightRotate(struct rbTree *t, struct rbTreeNode *x)
{
    struct rbTreeNode *y = l(x);
    x->left = r(y);
    if (r(y) != RB_NIL)
        r(y)->parent = x;
    y->parent = p(x);
    if (p(x) == RB_NIL)
        t->root = y;
    else if (x == r(p(x)))
        p(x)->right = y;
    else
        p(x)->left = y;
    y->right = x;
    x->parent = y;
}
static void rbTreeTransplant(struct rbTree *t, struct rbTreeNode *u, struct rbTreeNode *v)
{
    if (p(u) == RB_NIL)
        t->root = v;
    else if (u == l(p(u)))
        p(u)->left = v;
    else
        p(u)->right = v;
    if (v) // xxxx
        v->parent = p(u);
}
static struct rbTreeNode *rbTreeNodeFindMin(struct rbTreeNode *n)
{
    while (n && n != RB_NIL)
    {
        if (l(n) && l(n) != RB_NIL)
            n = l(n);
        else
            return n;
    }
    return NULL;
}
#ifdef DEBUG
static void rbTreeCheckParent(struct rbTreeNode *n)
{
    if (n && n != RB_NIL)
    {
        if (l(n) && l(n) != RB_NIL && p(l(n)) != n)
            printError("rbTree parent set error\n");
        if (r(n) && r(n) != RB_NIL && p(r(n)) != n)
            printError("rbTree parent set error\n");
    }
}
static void rbTreeCheckKey(struct rbTreeNode *n)
{
    if (n && n != RB_NIL)
    {
        if (l(n) && l(n) != RB_NIL && l(n)->key > n->key)
            printError("rbTree left node error\n");
        if (r(n) && r(n) != RB_NIL && r(n)->key < n->key)
            printError("rbTree right node error\n");
    }
}
static void rbTreeCheckRedColor(struct rbTreeNode *n)
{
    if (n && n != RB_NIL)
    {
        if (c(n) == RB_RED)
        {
            if (p(n) && p(n) != RB_NIL && c(p(n)) == RB_RED)
                printError("rbTree color error");
            if (l(n) && l(n) != RB_NIL && c(l(n)) == RB_RED)
                printError("rbTree color error");
            if (r(n) && r(n) != RB_NIL && c(r(n)) == RB_RED)
                printError("rbTree color error");
        }
    }
}
static int rbTreeLeafBlackColorNumber(struct rbTreeNode *n)
{
    int i = 0;
    while (n && n != RB_NIL)
    {
        if (c(n) == RB_BLACK)
            i++;
        n = p(n);
    }
    return i;
}
void rbTreePrint(struct rbTree *t, void (*printVal)(void *))
{
    if (t && t->root && t->root != RB_NIL && printVal)
    {
        struct Stack *stack = stackNew();
        if (!stack)
            goto freePointer;

        struct Stack *leafStack = stackNew();
        if (!leafStack)
            goto freePointer;

        stackPush(stack, t->root);
        struct rbTreeNode *printed = NULL;
        while (stackPeek(stack))
        {
            struct rbTreeNode *n = (struct rbTreeNode *)stackPop(stack);
            struct rbTreeNode *l = n && n->left != RB_NIL ? n->left : NULL;
            struct rbTreeNode *r = n && n->right != RB_NIL ? n->right : NULL;

            if (!l && !r)
                stackPush(leafStack, n);

            rbTreeCheckKey(n);
            rbTreeCheckParent(n);
            rbTreeCheckRedColor(n);

            if (l)
            {
                if (printed && printed->key >= l->key)
                {
                    printVal(n->val);
                    printed = n;
                    if (r)
                        stackPush(stack, r);
                }
                else
                {
                    stackPush(stack, n);
                    stackPush(stack, l);
                }
            }
            else
            {
                printVal(n->val);
                printed = n;
                if (r)
                    stackPush(stack, r);
            }
        }

        if (stackSize(leafStack))
        {
            struct rbTreeNode *leaf = (struct rbTreeNode *)stackPeek(leafStack);
            int blackNumber = rbTreeLeafBlackColorNumber(leaf);
            if (!blackNumber)
                printError("rbTree leaf black number error\n");
            if (stackSize(leafStack) == 1)
                if (blackNumber != 1)
                    printError("rbTree leaf black number error\n");

            while (stackPeek(leafStack))
            {
                leaf = (struct rbTreeNode *)stackPop(leafStack);
                if (blackNumber != rbTreeLeafBlackColorNumber(leaf))
                    printError("rbTree leaf black number error\n");
            }
        }
    freePointer:
        if (stack)
            stackFree(stack);
        if (leafStack)
            stackFree(leafStack);
    }
}
#endif

/*
 * ------------------------------------------------------------------------ List
 */

static struct listNode *listNodeNew(void *val);
static void listNodeFree();
static struct listNode *listGetNode(struct List *l, int i);
struct List *listNew()
{
    struct List *l = malloc(sizeof(struct List));
    if (l)
    {
        l->head = l->tail = NULL;
        l->size = 0;
        return l;
    }
    else
    {
        printError("listNew error\n");
        return NULL;
    }
}
void listFree(struct List *l)
{
    if (l)
    {
        struct listNode *c = l->head;
        struct listNode *n = NULL;
        while (c)
        {
            n = c->next;
            listNodeFree(c);
            c = n;
        }
        free(l);
    }
}
static struct listNode *listNodeNew(void *val)
{
    struct listNode *n = malloc(sizeof(struct listNode));
    if (n)
    {
        n->prev = n->next = NULL;
        n->val = val;
        return n;
    }
    else
    {
        printError("listNodeNew error\n");
        return NULL;
    }
}
static void listNodeFree(struct listNode *n)
{
    if (n)
        free(n);
}
int listAdd(struct List *l, void *el)
{
    if (!l)
    {
        printError("listAdd l is NULL\n");
        return 0;
    }

    struct listNode *n = listNodeNew(el);
    if (!n)
    {
        printError("listNodeNew error\n");
        return 0;
    }

    if (!l->head)
        l->head = n;
    if (l->tail)
    {
        n->prev = l->tail;
        l->tail->next = n;
    }
    l->tail = n;

    l->size++;
    return 1;
}
int listSet(struct List *l, int i, void *el)
{
    if (!l)
    {
        printError("listAdd l is NULL\n");
        return 0;
    }

    if (i >= l->size)
        return 0;

    listGetNode(l, i)->val = el;

    return 1;
}
int listRemove(struct List *l, int i)
{
    if (!l)
    {
        printError("listAdd l is NULL\n");
        return 0;
    }

    if (i >= l->size)
        return 0;

    struct listNode *n = listGetNode(l, i);
    if (n->prev)
        n->prev->next = n->next;
    if (n->next)
        n->next->prev = n->prev;
    if (n == l->head)
        l->head = n->next;
    if (n == l->tail)
        l->tail = n->prev;

    listNodeFree(n);

    l->size--;
    return 1;
}
void *listGet(struct List *l, int i)
{
    if (!l)
    {
        printError("listAdd l is NULL\n");
        return NULL;
    }

    if (i >= l->size)
        return NULL;

    return listGetNode(l, i)->val;
}
void *listHead(struct List *l)
{
    if (!l)
    {
        printError("listAdd l is NULL\n");
        return NULL;
    }
    return l->head ? l->head->val : NULL;
}
void *listTail(struct List *l)
{
    if (!l)
    {
        printError("listAdd l is NULL\n");
        return NULL;
    }
    return l->tail ? l->tail->val : NULL;
}
int listContains(struct List *l, void *el)
{
    if (!l)
    {
        printError("listAdd l is NULL\n");
        return 0;
    }
    if (l->size)
    {
        struct listNode *n = l->head;
        while (n)
        {
            if (n->val == el)
                return 1;
            else
                n = n->next;
        }
    }
    return 0;
}
int listSize(struct List *l)
{
    return l ? l->size : 0;
}
static struct listNode *listGetNode(struct List *l, int i)
{
    if ((i << 1) < l->size)
    {
        struct listNode *n = l->head;
        int j;
        for (j = 0; j < i; j++)
            n = n->next;
        return n;
    }
    else
    {
        struct listNode *n = l->tail;
        int j;
        for (j = l->size - 1; j > i; j--)
            n = n->prev;
        return n;
    }
}

/*
 * ------------------------------------------------------------------------ Dict
 */

static struct dictEntry *dictEntryNew(int hash, void *key, void *val);
static void dictEntryFree(struct dictEntry *e);
static int hash2(int hash);
static int tableIndex(int cap, int hash2);
static int resize(struct Dict *d);
static void rehash(struct Dict *d);
static struct dictEntry *dictGetEntry(struct Dict *d, void *key);
struct Dict *dictNew(int (*keyHash)(void *), int (*keyCompare)(void *, void *),
                     int (*valCompare)(void *, void *))
{
    if (!keyHash)
    {
        printError("dictNew keyHash is NULL\n");
        return NULL;
    }
    if (!keyCompare)
    {
        printError("dictNew keyCompare is NULL\n");
        return NULL;
    }
    if (!valCompare)
    {
        printError("dictNew valCompare is NULL\n");
        return NULL;
    }
    struct Dict *d = malloc(sizeof(struct Dict));
    if (d)
    {
        d->table = NULL;
        // factor = 0.75 = 3:4 = 6:8
        d->threshold = 6;
        d->cap = 8;
        d->size = 0;
        d->keyHash = keyHash;
        d->keyCompare = keyCompare;
        d->valCompare = valCompare;
        return d;
    }
    else
    {
        printError("dictNew error\n");
        return NULL;
    }
}
void dictFree(struct Dict *d)
{
    if (d)
    {
        if (d->table)
        {
            int i;
            for (i = 0; i < d->cap; i++)
            {
                struct dictEntry *c = *(d->table + i);
                struct dictEntry *n = NULL;
                while (c)
                {
                    n = c->next;
                    dictEntryFree(c);
                    c = n;
                }
            }

            free(d->table);
        }
        free(d);
    }
}
static struct dictEntry *dictEntryNew(int hash, void *key, void *val)
{
    struct dictEntry *entry = malloc(sizeof(struct dictEntry));
    if (entry)
    {
        entry->hash = hash;
        entry->key = key;
        entry->val = val;
        entry->next = NULL;
        return entry;
    }
    else
    {
        printError("dictEntryNew error\n");
        return NULL;
    }
}
static void dictEntryFree(struct dictEntry *entry)
{
    if (entry)
        free(entry);
}
int dictPut(struct Dict *d, void *key, void *val)
{
    if (!d)
    {
        printError("dictPut d is NULL\n");
        return 0;
    }
    if (!key)
    {
        printError("dictPut key is NULL\n");
        return 0;
    }
    if (d->size == 0)
    {
        if (d->table == NULL)
        {
            d->table = malloc(sizeof(struct dictEntry *) * d->cap);
            if (d->table)
            {
                int i;
                for (i = 0; i < d->cap; i++)
                    *(d->table + i) = NULL;
            }
            else
            {
                printError("dictPut init table error\n");
                return 0;
            }
        }
    }

    // maybe replace
    int h = d->keyHash(key);
    int i = tableIndex(d->cap, h);
    struct dictEntry *entry = *(d->table + i);
    while (entry)
    {
        if (entry->hash == h && !d->keyCompare(key, entry->key))
        {
            entry->val = val;
            return 1;
        }
        entry = entry->next;
    }

    int recap = 0;
    if (d->threshold == d->size)
    {
        recap = d->cap < (1 << 30);
        if (!resize(d))
        {
            printError("resize error\n");
            return 0;
        }
    }

    if (recap)
    {
        h = d->keyHash(key);
        i = tableIndex(d->cap, h);
    }
    entry = dictEntryNew(h, key, val);
    if (!entry)
    {
        printError("dictPut error\n");
        return 0;
    }
    entry->next = *(d->table + i);
    *(d->table + i) = entry;

    d->size++;
    return 1;
}
int dictRemove(struct Dict *d, void *key)
{
    if (!d)
    {
        printError("dictRemove d is NULL\n");
        return 0;
    }
    if (!key)
    {
        printError("dictRemove key is NULL\n");
        return 0;
    }

    if (d->size == 0)
        return 0;

    int h = d->keyHash(key);
    int i = tableIndex(d->cap, h);
    struct dictEntry *p = NULL;
    struct dictEntry *c = *(d->table + i);
    while (c)
    {
        if (c->hash == h && !d->keyCompare(c->key, key))
        {
            if (p)
                p->next = c->next;
            else
                *(d->table + i) = c->next;
            dictEntryFree(c);
            d->size--;
            return 1;
        }
        p = c;
        c = c->next;
    }
    return 0;
}
void *dictGet(struct Dict *d, void *key)
{
    if (!d)
    {
        printError("dictGet d is NULL\n");
        return NULL;
    }
    if (!key)
    {
        printError("dictGet key is NULL\n");
        return NULL;
    }
    struct dictEntry *entry = dictGetEntry(d, key);
    return entry ? entry->val : NULL;
}
int dictContainsKey(struct Dict *d, void *key)
{
    if (!d)
    {
        printError("dictGet d is NULL\n");
        return 0;
    }
    if (!key)
    {
        printError("dictGet key is NULL\n");
        return 0;
    }
    struct dictEntry *entry = dictGetEntry(d, key);
    return entry ? 1 : 0;
}
int dictContainsValue(struct Dict *d, void *val)
{
    if (!d)
    {
        printError("dictGet d is NULL\n");
        return 0;
    }
    if (!val)
    {
        printError("dictGet val is NULL\n");
        return 0;
    }
    if (d->size == 0)
        return 0;
    if (d->table)
    {
        int i;
        for (i = 0; i < d->cap; i++)
        {
            struct dictEntry *entry = *(d->table + i);
            while (entry)
            {
                if (!d->valCompare(entry->val, val))
                    return 1;
                entry = entry->next;
            }
        }
    }
    return 0;
}
int dictSize(struct Dict *d)
{
    return d ? d->size : 0;
}
#ifdef DEBUG
void dictPrint(struct Dict *d, void (*print)(void *, void *))
{
    if (!d)
    {
        printError("dictPrint d is NULL\n");
        return;
    }
    if (d->size)
    {
        int i;
        for (i = 0; i < d->cap; i++)
        {
            struct dictEntry *entry = *(d->table + i);
            while (entry)
            {
                print(entry->key, entry->val);
                entry = entry->next;
            }
        }
    }
}
#endif
static int hash2(int hash)
{
    int hash2 = hash < 0 ? -hash : hash;
    hash2 = hash2 ^ (hash2 >> 16);
    return hash2;
}
static int tableIndex(int cap, int hash)
{
    return hash2(hash) & (cap - 1);
}
static int resize(struct Dict *d)
{
    if (d->cap < (1 << 30))
    {
        int newCap = d->cap << 1;
        int newThreshold = d->threshold << 1;

        struct dictEntry **newTable = realloc(d->table, sizeof(struct dictEntry *) * newCap);
        if (!newTable)
        {
            printError("dict resize error\n");
            return 0;
        }

        int i;
        for (i = d->cap; i < newCap - 1; i++)
            *(newTable + i) = NULL;

        d->table = newTable;
        d->threshold = newThreshold;
        d->cap = newCap;

        rehash(d);

        return 1;
    }
    else if (d->threshold < INT_MAX)
    {
        d->threshold = INT_MAX;
        return 1;
    }
    else
    {
        printError("dict cap is not enough\n");
        return 0;
    }
}
static void rehash(struct Dict *d)
{
    // cap -1 = 2 ^ 3 - 1 = 111     hash2 = hash & 111
    // cap -1 = 2 ^ 4 - 1 = 1111    hash2 = hash & 1111
    // cap -1 = 2 ^ 5 - 1 = 11111   hash2 = hash & 11111
    // ...
    int i;
    for (i = (d->cap >> 1) - 1; i >= 0; i--)
    {
        struct dictEntry *h = *(d->table + i);
        struct dictEntry *p = NULL;
        struct dictEntry *c = *(d->table + i);
        struct dictEntry *n = NULL;
        while (c)
        {
            n = c->next;
            int j = tableIndex(d->cap, c->hash);
            if (j == i)
                p = c;
            else
            {
                c->next = *(d->table + j);
                *(d->table + j) = c;
                if (h == c)
                    h = *(d->table + i) = n;
                else
                    p->next = n;
            }
            c = n;
        }
    }
}
static struct dictEntry *dictGetEntry(struct Dict *d, void *key)
{
    if (d->size == 0)
        return NULL;

    int h = d->keyHash(key);
    int i = tableIndex(d->cap, h);
    struct dictEntry *entry = *(d->table + i);
    while (entry)
    {
        if (entry->hash == h && !d->keyCompare(key, entry->key))
            return entry;
        entry = entry->next;
    }
    return NULL;
}

/*
 * ----------------------------------------------------------------- binary heap
 */

static int bhp(int i);
static int bhl(int i);
static int bhr(int i);
struct binaryHeap *bhNew(int (*key)(void *))
{
    if (!key)
    {
        printError("bhNew key is NULL\n");
        return NULL;
    }
    struct binaryHeap *h = malloc(sizeof(struct binaryHeap));
    if (h)
    {
        h->table = NULL;
        h->cap = 8;
        h->size = 0;
        h->key = key;
        return h;
    }
    else
    {
        printError("bhNew error\n");
        return NULL;
    }
}
void bhFree(struct binaryHeap *h)
{
    if (h)
    {
        if (h->table)
            free(h->table);
        free(h);
    }
}
int bhInsert(struct binaryHeap *h, void *el)
{
    if (!h)
    {
        printError("bhSearch h is NULL\n");
        return 0;
    }
    if (!el)
    {
        printError("bhSearch el is NULL\n");
        return 0;
    }
    if (!h->table)
    {
        h->table = malloc(sizeof(void *) * h->cap);
        if (!h->table)
        {
            printError("bhSearch init table error\n");
            return 0;
        }
    }

    if ((h->size + 1) == h->cap)
    {
        int newCap = h->cap << 1;
        void **newTable = realloc(h->table, sizeof(void *) * newCap);
        if (!newTable)
        {
            printError("bhInsert error\n");
            return 0;
        }
        h->cap = newCap;
        h->table = newTable;
    }

    int k = h->key(el);
    int i = h->size + 1;
    while (i > 1)
    {
        int p = bhp(i);
        if (k < h->key(*(h->table + p)))
        {
            *(h->table + i) = *(h->table + p);
            i = p;
        }
        else
            break;
    }
    *(h->table + i) = el;
    h->size++;
    return 1;
}
int bhDeleteMin(struct binaryHeap *h)
{
    if (!h)
    {
        printError("bhSearch h is NULL\n");
        return 0;
    }
    if (!h->size)
        return 0;

    h->size--;

    if (h->size)
    {
        *(h->table + 1) = *(h->table + h->size + 1);
        int k = h->key(*(h->table + 1));
        int i = 1;
        for (;;)
        {
            int l = bhl(i);
            int r = bhr(i);
            int x;
            int xk;
            if (l <= h->size && r <= h->size)
            {
                int lk = h->key(*(h->table + l));
                int rk = h->key(*(h->table + r));
                x = lk < rk ? l : r;
                xk = lk < rk ? lk : rk;
            }
            else if (l <= h->size)
            {
                x = l;
                xk = h->key(*(h->table + l));
            }
            else if (r <= h->size)
            {
                x = r;
                xk = h->key(*(h->table + r));
            }
            else
                break;

            if (k > xk)
            {
                void *c = *(h->table + i);
                *(h->table + i) = *(h->table + x);
                *(h->table + x) = c;
                i = x;
            }
            else
                break;
        }
    }

    return 1;
}
void *bhFindMin(struct binaryHeap *h)
{
    if (!h)
    {
        printError("bhFindMin h is NULL\n");
        return NULL;
    }
    if (h->size == 0)
        return NULL;
    return *(h->table + 1);
}
void *bhFindMax(struct binaryHeap *h)
{
    if (!h)
    {
        printError("bhFindMax h is NULL\n");
        return NULL;
    }
    if (h->size == 0)
        return NULL;
    int i = (h->size >> 1) + 1;
    int maxKey;
    void *max = NULL;
    for (; i <= h->size; i++)
    {
        void *el = *(h->table + i);
        if (max)
        {
            int k = h->key(el);
            if (k > maxKey)
            {
                maxKey = k;
                max = el;
            }
        }
        else
        {
            maxKey = h->key(el);
            max = el;
        }
    }
    return max;
}
int bhSize(struct binaryHeap *h)
{
    if (!h)
    {
        printError("bhFindMax h is NULL\n");
        return 0;
    }
    return h->size;
}
#ifdef DEBUG
void bhPrint(struct binaryHeap *h, void (*print)(void *))
{
    if (!h)
    {
        printError("bhPrint h is NULL\n");
        return;
    }
    if (h->size)
    {
        int i;
        for (i = 1; i <= h->size; i++)
        {
            print(*(h->table + i));

            int k = h->key(*(h->table + i));

            int l = bhl(i);
            if (l <= h->size)
            {
                int lk;
                if (k > (lk = h->key(*(h->table + l))))
                {
                    printf("\n");
                    printError("bhPrint check left error, k=%d,lk=%d\n", k, lk);
                    return;
                }
            }

            int r = bhr(i);
            if (r <= h->size)
            {
                int rk;
                if (k > (rk = h->key(*(h->table + r))))
                {
                    printf("\n");
                    printError("bhPrint check right error, k=%d,rk=%d\n", k, rk);
                    return;
                }
            }
        }
        printf("\n");
    }
}
#endif
static int bhp(int i)
{
    return i >> 1;
}
static int bhl(int i)
{
    return i << 1;
}
static int bhr(int i)
{
    return (i << 1) + 1;
}

/*
 * ------------------------------------------------------------------- Skip List
 */

static struct skipListNode *skipListNodeNew(int maxLevel, int key, void *val);
static void skipListNodeFree(struct skipListNode *n);
static int skipListInsertCore(struct skipList *sl, struct skipListNode *n);
static int skipListDeleteCore(struct skipList *sl,
                              struct skipListNode *p, struct skipListNode *c);
static struct skipListNode *skipListGetCore(struct skipList *sl, int key);
static int skipListHeadMaxLevel(struct skipList *sl);
static int randomLevel(struct skipList *sl);
struct skipList *skipListNew(int (*key)(void *))
{
    if (!key)
    {
        printError("skipListNew key is NULL\n");
        return 0;
    }
    struct skipList *sl = malloc(sizeof(struct skipList));
    if (sl)
    {
        sl->time0 = time(0);
        sl->maxLevel = 0;
        sl->head = NULL;
        sl->size = 0;
        sl->key = key;
        return sl;
    }
    printError("skipListNew error\n");
    return NULL;
}
void skipListFree(struct skipList *sl)
{
    if (sl)
    {
        struct skipListNode *c = sl->head;
        struct skipListNode *n = NULL;
        while (c)
        {
            if (c->next)
                n = *(c->next + 0);
            else
                n = NULL;
            skipListNodeFree(c);
            c = n;
        }
        free(sl);
    }
}
static struct skipListNode *skipListNodeNew(int maxLevel, int key, void *val)
{
    struct skipListNode *n = malloc(sizeof(struct skipListNode));
    if (n)
    {
        n->maxLevel = maxLevel;
        n->next = calloc(maxLevel + 1, sizeof(struct skipListNode *));
        if (!n->next)
            goto newError;
        n->key = key;
        n->val = val;
        return n;
    }
newError:
    if (n)
        free(n);
    printError("skipListNodeNew error\n");
    return NULL;
}
static void skipListNodeFree(struct skipListNode *n)
{
    if (n)
    {
        if (n->next)
            free(n->next);
        free(n);
    }
}
int skipListInsert(struct skipList *sl, void *el)
{
    if (!sl)
    {
        printError("skipListInsert sl is NULL\n");
        return 0;
    }
    if (!el)
    {
        printError("skipListInsert el is NULL\n");
        return 0;
    }

    int key = sl->key(el);

    if (sl->size)
    {
        struct skipListNode *n = skipListGetCore(sl, key);
        if (n)
        {
            n->val = el;
            return 1;
        }

        int level = randomLevel(sl);
        n = skipListNodeNew(level, key, el);
        if (!n)
        {
            printError("skipListInsert error\n");
            return 0;
        }

        return skipListInsertCore(sl, n);
    }
    else
    {
        // head node maxLevel is SL_MAX_LEVEL - 1
        struct skipListNode *n = skipListNodeNew(SL_MAX_LEVEL - 1, key, el);
        if (!n)
        {
            printError("skipListInsert error\n");
            return 0;
        }

        sl->maxLevel = 0;
        sl->head = n;
        sl->size++;

        return 1;
    }
}
static int skipListInsertCore(struct skipList *sl, struct skipListNode *n)
{
    int level = n->maxLevel;
    int key = n->key;
    void *val = n->val;

    if (key < sl->head->key)
    {
        n->key = sl->head->key;
        n->val = sl->head->val;
        sl->head->key = key;
        sl->head->val = val;
        key = n->key;
        val = n->val;
    }

    // do insert sl node
    struct skipListNode *c = sl->head;
    struct skipListNode *cn = NULL;
    while (c && level >= 0)
    {
        if (c->key < key)
        {
            if ((cn = *(c->next + level)))
            {
                if (cn->key > key)
                {
                    // do insert [c, n, cn]
                    *(c->next + level) = n;
                    *(n->next + level) = cn;
                    level--;
                }
                else
                    c = cn;
            }
            else
            {
                // do insert [c, n]
                *(c->next + level) = n;
                level--;
            }
        }
        else
            break;
    }

    sl->size++;

    sl->maxLevel = skipListHeadMaxLevel(sl);

    return 1;
}
int skipListDelete(struct skipList *sl, int key)
{
    if (!sl)
    {
        printError("skipListDelete sl is NULL\n");
        return 0;
    }

    if (!sl->size)
        return 0;

    int level = sl->maxLevel;
    struct skipListNode *p = NULL;
    struct skipListNode *c = sl->head;
    struct skipListNode *n = NULL;
    while (c)
    {
        if (c->key == key)
            return skipListDeleteCore(sl, p, c);
        else if ((n = *(c->next + level)))
        {
            if (n->key < key)
            {
                p = c;
                c = n;
            }
            else if (n->key > key)
            {
                if (level)
                    level--;
                else
                    return 0;
            }
            else
                return skipListDeleteCore(sl, c, n);
        }
        else
        {
            if (level)
                level--;
            else
                return 0;
        }
    }
    return 0;
}
static int skipListDeleteCore(struct skipList *sl,
                              struct skipListNode *p, struct skipListNode *c)
{
    if (sl->size == 1)
    {
        skipListNodeFree(c);
        sl->head = NULL;
    }
    else
    {
        if (c == sl->head)
        {
            struct skipListNode *n = *(c->next + 0);

            c->key = n->key;
            c->val = n->val;

            p = c; // c is a head node, the node n just has one prev node
            c = n;
        }

        int i;
        for (i = c->maxLevel; i >= 0; i--)
        {
            struct skipListNode *n = NULL;
            while (p)
            {
                if ((n = *(p->next + i)) == c)
                {
                    *(p->next + i) = *(c->next + i);
                    break;
                }
                else
                    p = n;
            }
        }

        skipListNodeFree(c);
    }

    sl->size--;

    sl->maxLevel = skipListHeadMaxLevel(sl);

    return 1;
}
void *skipListGet(struct skipList *sl, int key)
{
    if (!sl)
    {
        printError("sl is NULL\n");
        return NULL;
    }
    if (!sl->size)
        return NULL;

    struct skipListNode *n = skipListGetCore(sl, key);
    return n ? n->val : NULL;
}
static struct skipListNode *skipListGetCore(struct skipList *sl, int key)
{
    if (!sl->size)
        return NULL;

    int level = sl->maxLevel;
    struct skipListNode *c = sl->head;
    struct skipListNode *n = NULL;
    while (c)
    {
        if (c->key == key)
            return c;
        else if ((n = *(c->next + level)))
        {
            if (n->key < key)
                c = n;
            else if (n->key > key)
            {
                if (level)
                    level--;
                else
                    return NULL;
            }
            else
                return n;
        }
        else
        {
            if (level)
                level--;
            else
                return NULL;
        }
    }
    return NULL;
}
int skipListSize(struct skipList *sl)
{
    return sl ? sl->size : 0;
}
static int skipListHeadMaxLevel(struct skipList *sl)
{
    if (sl->size)
    {
        int maxLevel = 0;
        while (maxLevel < SL_MAX_LEVEL && *(sl->head->next + maxLevel))
            maxLevel++;
        return max(maxLevel - 1, 0);
    }
    else
        return -1;
}
static int randomLevel(struct skipList *sl)
{
    sl->time0++;
    srand(sl->time0);
    int mask = 127;
    int half = 64;
    int level = 0;
    while ((rand() & mask) < half)
        level++;
    level = level < SL_MAX_LEVEL ? level : SL_MAX_LEVEL - 1;
    return level;
}
#ifdef DEBUG
void skipListPrint(struct skipList *sl, void (*print)(void *))
{
    if (sl->size)
    {
        int level = SL_MAX_LEVEL - 1; // sl->maxLevel;
        // not include head
        int levelLinkedNodeTotal[level + 1];
        // init all element value 0
        int i;
        for (i = 0; i < level + 1; i++)
            levelLinkedNodeTotal[i] = 0;

        struct skipListNode *n = sl->head;
        while (n)
        {
            if (n != sl->head)
                for (i = 0; i <= n->maxLevel; i++)
                    levelLinkedNodeTotal[i]++;
            n = *(n->next + 0);
        }

        for (i = 0; i < level + 1; i++)
            printDebug("level %d linked node total: %d\n", i, levelLinkedNodeTotal[i]);

        for (; level >= 0; level--)
        {
            printDebug("skipListPrint check level linked node total: %d\n", level);

            int total = 0;
            struct skipListNode *p = NULL;
            struct skipListNode *c = sl->head;
            while (c)
            {
                if (c != sl->head)
                    total++;
                if (p && p->key > c->key)
                {
                    printError("skipListPrint linked node sort(prev: %d, curr: %d) error\n",
                               p->key, c->key);
                    return;
                }
                p = c;
                c = *(c->next + level);
            }
            if (total != levelLinkedNodeTotal[level])
            {
                printError("skipListPrint level: %d, linked node total: %d <> %d error\n",
                           level, levelLinkedNodeTotal[level], total);
                return;
            }
        }
    }
}
#endif

/*
 * --------------------------------------------------------------------- Bit Set
 */

static int bitSetIndex(int i);
static int bitSetBit(int i);
static int bitSetResize(struct bitSet *bs, int index);
struct bitSet *bitSetNew()
{
    struct bitSet *bs = malloc(sizeof(struct bitSet));
    if (bs)
    {
        bs->size = 0;
        bs->els = NULL;
        return bs;
    }
    else
    {
        printError("bitSetNew error\n");
        return NULL;
    }
}
void bitSetFree(struct bitSet *bs)
{
    if (bs)
    {
        if (bs->els)
            free(bs->els);
        free(bs);
    }
}
int bitSetOn(struct bitSet *bs, int i)
{
    if (!bs)
    {
        printError("bitSetOn bs is NULL\n");
        return 0;
    }
    if (i < 0)
    {
        printError("bitSetOn i is error\n");
        return 0;
    }
    int index = bitSetIndex(i);
    if (!bitSetResize(bs, index))
    {
        printError("bitSetOn resize error\n");
        return 0;
    }
    int bit = bitSetBit(i);
    *(bs->els + index) = (*(bs->els + index)) | (((UINT64)1) << bit);
    return 1;
}
int bitSetOff(struct bitSet *bs, int i)
{
    if (!bs)
    {
        printError("bitSetOff bs is NULL\n");
        return 0;
    }
    if (i < 0)
    {
        printError("bitSetOff i is error\n");
        return 0;
    }
    int index = bitSetIndex(i);
    if (!bitSetResize(bs, index))
    {
        printError("bitSetOff resize error\n");
        return 0;
    }
    int bit = bitSetBit(i);
    *(bs->els + index) = (*(bs->els + index)) & (ULONG_MAX ^ (((UINT64)1) << bit));
    return 1;
}
int bitSetGet(struct bitSet *bs, int i)
{
    if (!bs)
    {
        printError("bitSetOff bs is NULL\n");
        return 0;
    }
    if (i < 0)
    {
        printError("bitSetOff i is error\n");
        return 0;
    }
    int index = bitSetIndex(i);
    if (index >= bs->size)
        return 0;
    int bit = bitSetBit(i);
    return ((*(bs->els + index)) & (((UINT64)1) << bit)) ? 1 : 0;
}
#ifdef DEBUG
void bitSetPrint(struct bitSet *bs)
{
    if (!bs)
        return;
    if (!bs->size)
        return;
    int i;
    for (i = 0; i < 64; i++)
    {
        if (i % 4 == 0)
        {
            if (i < 10)
                printf("%d    ", i);
            else
                printf("%d   ", i);
        }
    }
    printf("\n");
    for (i = 0; i < bs->size; i++)
    {
        UINT64 x = *(bs->els + i);
        int j;
        for (j = 0; j < 64; j++)
        {
            if ((x & (((UINT64)1) << j)))
                putchar('1');
            else
                putchar('0');
            if ((j + 1) % 4 == 0)
                putchar(' ');
        }
        putchar('\n');
    }
}
#endif
static int bitSetIndex(int i)
{
    return i / 64;
}
static int bitSetBit(int i)
{
    return i % 64;
}
static int bitSetResize(struct bitSet *bs, int index)
{
    int newSize = index + 1;
    if (newSize > INT_MAX)
    {
        printError("bitSetResize index is too large\n");
        return 0;
    }
    if (newSize <= bs->size)
        return 1;
    if (bs->els)
    {
        UINT64 *newEls = (UINT64 *)realloc(bs->els, sizeof(UINT64) * newSize);
        if (newEls)
        {
            if (newSize > bs->size)
            {
                int i;
                for (i = bs->size; i < newSize; i++)
                    *(bs->els + i) = 0;
            }
            bs->els = newEls;
            bs->size = newSize;
            return 1;
        }
    }
    else
    {
        bs->els = calloc(newSize, sizeof(UINT64));
        if (bs->els)
        {
            bs->size = newSize;
            return 1;
        }
    }

    printError("bitSetResize error\n");
    return 0;
}
