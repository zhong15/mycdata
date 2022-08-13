void printError(const char *msg, ...);
void printWarn(const char *msg, ...);
void printInfo(const char *msg, ...);
void printDebug(const char *msg, ...);

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
