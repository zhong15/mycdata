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
 * ---------------------------------------------- Red-Black Tree
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
