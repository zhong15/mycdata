#include <stdio.h>
#include <stdlib.h>
#include "mycdata.h"

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