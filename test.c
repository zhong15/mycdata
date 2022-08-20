#include <stdio.h>
#include <stdlib.h>
#include "mycdata.h"

void test_print();
void test_stack();
void test_queue();
void test_avlTree();
void test_avlTree2();
void test_rbTree();
void test_rbTree2();

int main(int argc, char **argv)
{
    test_print();
    test_stack();
    test_queue();
    test_avlTree();
    test_avlTree2();
    test_rbTree();
    test_rbTree2();
}

void test_print()
{
    printError("Hello, %s %d\n", "World!", 123);
    printWarn("Hello, %s %d\n", "World!", 123);
    printInfo("Hello, %s %d\n", "World!", 123);
    printDebug("Hello, %s %d\n", "World!", 123);
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

int test_avlTreeIntKey(void *val)
{
    return *(int *)val;
}
void test_avlTreePrintfInt(void *val)
{
    printf("%d ", *(int *)val);
}
void test_avlTree()
{
    struct avlTree *tree = avlTreeNew(&test_avlTreeIntKey);
    if (!tree)
    {
        printError("create tree error\n");
        return;
    }

    printInfo("===============\n");

    int nums[] = {110, 140, 120, 130, 145, 150, 250, 50, 100, 25, 77, 99, 33, 1, 2, 3, 4, 0, 8, 9};
    int i;
    for (i = 0; i < 20; i++)
    {
        printInfo("add %2d %4d: ", i, nums[i]);
        avlTreeAdd(tree, &nums[i]);
        avlTreePrint(tree, test_avlTreePrintfInt);
        printf("\n");
    }

    printInfo("==============\n");

    int nums2[] = {10, 110, 5, 120, 11, 140, 130, 2, 15, 3, 4, 7, 6, 145, 1, 14, 12, 8, 13, 9, 0};
    for (i = 0; i < 20; i++)
    {
        printInfo("rm  %2d %4d: ", i, nums[i]);
        avlTreeRemove(tree, &nums[i]);
        avlTreePrint(tree, test_avlTreePrintfInt);
        printf("\n");
    }

    avlTreeFree(tree);

    printInfo("==============\n");
}
void test_avlTreePrintfInt2(void *val)
{
    // printf("%d ", *(int *)val);
}
void test_avlTree2()
{
    /* Copy from <<Data Structures and Algorithm Analysis in Java>> */

    struct avlTree *tree = avlTreeNew(&test_avlTreeIntKey);
    if (!tree)
    {
        printError("create tree error\n");
        return;
    }

    int SMALL = 40;
    int NUMS = 10000; // must be even
    int GAP = 37;

    printInfo("Checking... (no more output means success)\n");

    int *nums = malloc(sizeof(int) * (NUMS + 1));
    if (!nums)
    {
        printError("create nums error\n");
        return;
    }

    int i = 0;
    for (i = GAP; i != 0; i = (i + GAP) % NUMS)
    {
        *(nums + i) = i;
        //    System.out.println( "INSERT: " + i );
        avlTreeAdd(tree, nums + i);
        // if (NUMS < SMALL)
        avlTreePrint(tree, test_avlTreePrintfInt2);
    }

    for (i = 1; i < NUMS; i += 2)
    {
        //   System.out.println( "REMOVE: " + i );
        avlTreeRemove(tree, nums + i);
        // if (NUMS < SMALL)
        avlTreePrint(tree, test_avlTreePrintfInt2);
    }

    // if (NUMS < SMALL)
    //     t.printTree();
    avlTreePrint(tree, test_avlTreePrintfInt2);

    int *min = (int *)avlTreeFindMin(tree);
    int *max = (int *)avlTreeFindMax(tree);
    if (!min || (*min) != 2 || !max || (*max) != NUMS - 2)
        printError("FindMin or FindMax error!");

    for (i = 2; i < NUMS; i += 2)
        if (!avlTreeSearch(tree, nums + i))
            printError("Find error1!");

    for (i = 1; i < NUMS; i += 2)
        if (avlTreeSearch(tree, nums + i))
            printError("Find error2!");

    free(nums);

    avlTreeFree(tree);
}

int test_rbTreeIntKey(void *val)
{
    return *(int *)val;
}
void test_rbTreePrintfInt(void *val)
{
    if (val)
        printf("%d ", *(int *)val);
}
void test_rbTreePrintfInt2(void *val)
{
    // printf("%d ", *(int *)val);
}
void test_rbTree()
{
    struct rbTree *tree = rbTreeNew(&test_rbTreeIntKey);
    if (!tree)
    {
        printError("rbTreeNew error\n");
        return;
    }

    printInfo("===============\n");

    int nums[] = {110, 140, 120, 130, 145, 150, 250, 50, 100, 25, 77, 99, 33, 1, 2, 3, 4, 0, 8, 9};
    int i;
    for (i = 0; i < 20; i++)
    {
        printInfo("ins %2d %4d: ", i, nums[i]);
        rbTreeInsert(tree, &nums[i]);
        rbTreePrint(tree, test_rbTreePrintfInt);
        printf("\n");
    }

    printInfo("==============\n");

    int nums2[] = {10, 110, 5, 120, 11, 140, 130, 2, 15, 3, 4, 7, 6, 145, 1, 14, 12, 8, 13, 9, 0};
    for (i = 0; i < 20; i++)
    {
        printInfo("del  %2d %4d: ", i, nums[i]);
        rbTreeDelete(tree, &nums[i]);
        rbTreePrint(tree, test_rbTreePrintfInt);
        printf("\n");
    }

    rbTreeFree(tree);
}
void test_rbTree2()
{
    /* Copy from <<Data Structures and Algorithm Analysis in Java>> */

    printInfo("==============\n");

    struct rbTree *t = rbTreeNew(test_rbTreeIntKey);
    if (!t)
    {
        printError("rbTreeNew error\n");
        goto freePointer;
    }

    // RedBlackTree<Integer> t = new RedBlackTree<>();
    int NUMS = 400000; // 400000;
    int GAP = 35461;

    printInfo("Checking... (no more output means success)\n");

    int *nums = malloc(sizeof(int) * (NUMS + 1));
    if (!nums)
    {
        printError("malloc nums error\n");
        goto freePointer;
    }

    printInfo("one\n");

    int i;
    int operatorNum = 0;
    for (i = GAP; i != 0; i = (i + GAP) % NUMS)
    {
        operatorNum++;
        if (operatorNum % 10000 == 0)
            printInfo("one %d \n", operatorNum);

        // t.insert(i);
        *(nums + i) = i;
        rbTreeInsert(t, nums + i);
        // rbTreePrint(t, test_rbTreePrintfInt2);
    }
    rbTreePrint(t, test_rbTreePrintfInt2);

    printf("\n");
    printInfo("two\n");

    if ((*((int *)rbTreeFindMin(t))) != 1 || (*((int *)rbTreeFindMax(t))) != NUMS - 1)
        printError("FindMin or FindMax error!\n");

    printf("\n");
    printInfo("three\n");

    operatorNum = 0;
    for (i = 1; i < NUMS; i++)
    {
        operatorNum++;
        if (operatorNum % 10000 == 0)
            printInfo("three %d \n", operatorNum);

        if (!rbTreeSearch(t, nums + i))
            printError("Find error1!\n");
    }

    printf("\n");
    printInfo("four\n");

    operatorNum = 0;
    for (i = GAP; i != 0; i = (i + GAP) % NUMS)
    {
        operatorNum++;
        if (operatorNum % 999 == 0 || operatorNum % 10000 == 0)
        {
            // printInfo("four %d \n", operatorNum);

            rbTreeDelete(t, nums + i);
            rbTreePrint(t, test_rbTreePrintfInt2);
        }
    }

    printf("\n");
    printInfo("two 2\n");

    if ((*((int *)rbTreeFindMin(t))) != 1 || (*((int *)rbTreeFindMax(t))) != NUMS - 1)
        printError("FindMin or FindMax error!\n");

    printf("\n");
    printInfo("three 2\n");

    operatorNum = 0;
    // for (i = 1; i < NUMS; i++)
    for (i = GAP; i != 0; i = (i + GAP) % NUMS)
    {
        operatorNum++;
        if (operatorNum % 10000 == 0)
            printInfo("three 2 %d \n", operatorNum);

        if (operatorNum % 999 == 0 || operatorNum % 10000 == 0)
            continue;

        if (!rbTreeSearch(t, nums + i))
            printError("Find error1!\n");
    }

freePointer:
    if (nums)
        free(nums);
    if (t)
        rbTreeFree(t);

    printInfo("end\n");
}
