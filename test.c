#include <stdio.h>
#include <stdlib.h>
#include "mycdata.h"

void test_stack();
void test_queue();

int main(int argc, char **argv)
{
    test_stack();
    test_queue();
}

void test_stack()
{
    struct Stack *stack = stackNew();
    if (stack == NULL)
    {
        printf("create stack fail!\n");
        return;
    }
    else
        printf("create stack success!\n");

    int nums[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int i;
    for (i = 0; i < 10; i++)
    {
        int num, val, size;
        num = stackPush(stack, &nums[i]);
        val = *(int *)stackPeek(stack);
        size = stackSize(stack);
        printf("push num: %d, peek val: %d, size: %d\n", num, val, size);
    }

    for (i = 0; i < 10; i++)
    {
        int val, size;
        val = *(int *)stackPop(stack);
        size = stackSize(stack);
        printf("pop val: %d, size: %d\n", val, size);
    }

    stackFree(stack);
    printf("free stack success!\n");
    return;
}

void test_queue()
{
    struct Queue *queue = queueNew();
    if (queue == NULL)
    {
        printf("create queue fail!\n");
        return;
    }
    else
        printf("create queue success!\n");

    int nums[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int i;
    for (i = 0; i < 10; i++)
    {
        int num, val, size;
        num = queueOffer(queue, &nums[i]);
        val = *(int *)queuePeek(queue);
        size = queueSize(queue);
        printf("offer num: %d, peek val: %d, size: %d\n", num, val, size);
    }

    printf("=========================\n");

    for (i = 0; i < 3; i++)
    {
        int val, size;
        val = *(int *)queuePoll(queue);
        size = queueSize(queue);
        printf("poll val: %d, size: %d\n", val, size);
    }

    printf("=========================\n");

    int nums2[] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    for (i = 0; i < 10; i++)
    {
        int num, val, size;
        num = queueOffer(queue, &nums2[i]);
        val = *(int *)queuePeek(queue);
        size = queueSize(queue);
        printf("offer num: %d, peek val: %d, size: %d\n", num, val, size);
    }

    printf("=========================\n");

    const int size = queueSize(queue);
    for (i = 0; i < size; i++)
    {
        int val, size;
        val = *(int *)queuePoll(queue);
        size = queueSize(queue);
        printf("poll val: %d, size: %d\n", val, size);
    }

    queueFree(queue);
    printf("free queue success!\n");
    return;
}
