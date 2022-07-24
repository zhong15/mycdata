#include <stdio.h>
#include <stdlib.h>
#include "mycdata.h"

int main(int argc, char **argv)
{
    struct Stack *stack = stackNew();
    if (stack == NULL)
    {
        printf("create stack fail!\n");
        return 0;
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
    return 0;
}
