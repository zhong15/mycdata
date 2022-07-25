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
