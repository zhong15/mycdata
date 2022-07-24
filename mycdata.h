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
