#ifndef MYCDATA_H_
#define MYCDATA_H_

#ifndef RB_RED
#define RB_RED 1
#endif

#ifndef RB_BLACK
#define RB_BLACK 0
#endif

#ifndef SL_MAX_LEVEL
#define SL_MAX_LEVEL 64
#endif

/*
 * ---------------------------------------------- Print Message
 */

void printError(const char *msg, ...);
void printWarn(const char *msg, ...);
void printInfo(const char *msg, ...);
void printDebug(const char *msg, ...);

/*
 * ---------------------------------------------- Stack
 */

struct Stack
{
    void **els;
    int cap;
    int size;
};
struct Stack *stackNew();
void stackFree(struct Stack *p);
int stackPush(struct Stack *p, void *el);
void *stackPop(struct Stack *p);
void *stackPeek(struct Stack *p);
int stackSize(struct Stack *p);
void stackClear(struct Stack *p);

/*
 * ---------------------------------------------- Queue
 */

struct Queue
{
    void **els;
    int cap;
    int size;
    int head;
    int tail;
};
struct Queue *queueNew();
void queueFree(struct Queue *p);
int queueOffer(struct Queue *p, void *el);
void *queuePoll(struct Queue *p);
void *queuePeek(struct Queue *p);
int queueSize(struct Queue *p);
void queueClear(struct Queue *p);

/*
 * ---------------------------------------------- Avl Tree
 */

struct avlTreeNode
{
    struct avlTreeNode *parent, *left, *right;
    int key, height;
    void *val;
};
struct avlTree
{
    struct avlTreeNode *root;
    int size;
    int (*key)(void *);
};
struct avlTree *avlTreeNew(int (*key)(void *));
void avlTreeFree(struct avlTree *p);
int avlTreeAdd(struct avlTree *p, void *el);
int avlTreeRemove(struct avlTree *p, void *el);
void *avlTreeSearch(struct avlTree *p, void *el);
void *avlTreeFindMin(struct avlTree *p);
void *avlTreeFindMax(struct avlTree *p);
#ifdef DEBUG
void avlTreePrint(struct avlTree *p, void (*printVal)(void *));
#endif

/*
 * ---------------------------------------------- Red-Black Tree
 */

/* copy from CLRS 3 */
struct rbTreeNode
{
    struct rbTreeNode *parent;
    struct rbTreeNode *left;
    struct rbTreeNode *right;
    int color;
    int key;
    void *val;
};
struct rbTree
{
    struct rbTreeNode *root;
    int size;
    int (*key)(void *);
};
struct rbTree *rbTreeNew(int (*key)(void *));
void rbTreeFree(struct rbTree *t);
int rbTreeInsert(struct rbTree *t, void *el);
int rbTreeDelete(struct rbTree *t, void *el);
void *rbTreeSearch(struct rbTree *t, void *el);
void *rbTreeFindMin(struct rbTree *t);
void *rbTreeFindMax(struct rbTree *t);
static struct rbTreeNode RB_NIL2;
static struct rbTreeNode *RB_NIL = &RB_NIL2;
#ifdef DEBUG
void rbTreePrint(struct rbTree *t, void (*printVal)(void *));
#endif

/*
 * ---------------------------------------------- List
 */

struct listNode
{
    struct listNode *prev;
    struct listNode *next;
    void *val;
};
struct List
{
    struct listNode *head;
    struct listNode *tail;
    int size;
};
struct List *listNew();
void listFree(struct List *l);
int listAdd(struct List *l, void *el);
int listSet(struct List *l, int i, void *el);
int listRemove(struct List *l, int i);
void *listGet(struct List *l, int i);
void *listHead(struct List *l);
void *listTail(struct List *l);
int listContains(struct List *l, void *el);
int listSize(struct List *l);

/*
 * ---------------------------------------------- Dict
 */

struct dictEntry
{
    int hash;
    void *key;
    void *val;
    struct dictEntry *next;
};
struct Dict
{
    struct dictEntry **table;
    int threshold;
    int cap;
    int size;
    int (*keyHash)(void *);
    int (*keyCompare)(void *, void *);
    int (*valCompare)(void *, void *);
};
struct Dict *dictNew(int (*keyHash)(void *), int (*keyCompare)(void *, void *),
                     int (*valCompare)(void *, void *));
void dictFree(struct Dict *d);
int dictPut(struct Dict *d, void *key, void *val);
int dictRemove(struct Dict *d, void *key);
void *dictGet(struct Dict *d, void *key);
int dictContainsKey(struct Dict *d, void *key);
int dictContainsValue(struct Dict *d, void *val);
int dictSize(struct Dict *d);
#ifdef DEBUG
void dictPrint(struct Dict *d, void (*print)(void *, void *));
#endif

/*
 * ---------------------------------------------- binary heap
 */

struct binaryHeap
{
    void **table;
    int cap;
    int size;
    int (*key)(void *);
};
struct binaryHeap *bhNew(int (*key)(void *));
void bhFree(struct binaryHeap *h);
int bhInsert(struct binaryHeap *h, void *el);
int bhDeleteMin(struct binaryHeap *h);
void *bhFindMin(struct binaryHeap *h);
void *bhFindMax(struct binaryHeap *h);
int bhSize(struct binaryHeap *h);
#ifdef DEBUG
void bhPrint(struct binaryHeap *h, void (*print)(void *));
#endif

/*
 * ---------------------------------------------- Skip List
 */

struct skipListNode
{
    int maxLevel;
    struct skipListNode **next;
    int key;
    void *val;
};
struct skipList
{
    int time0;
    int maxLevel;
    struct skipListNode *head;
    int size;
    int (*key)(void *);
};
struct skipList *skipListNew(int (*key)(void *));
void skipListFree(struct skipList *sl);
int skipListInsert(struct skipList *sl, void *el);
int skipListDelete(struct skipList *sl, int key);
void *skipListGet(struct skipList *sl, int key);
int skipListSize(struct skipList *sl);
#ifdef DEBUG
void skipListPrint(struct skipList *sl, void (*print)(void *));
#endif

#endif
