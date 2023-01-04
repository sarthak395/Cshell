#include "headers.h"
#include "history.h"

// IMPLEMENTING QUEUE
Queue *createqueue()
{
    Queue *newq = (Queue *)malloc(sizeof(Queue));
    newq->capacity = 20; // DEFAULT VALUE
    newq->head = NULL;
    newq->rear = NULL;
    newq->size = 0;

    return newq;
}

Queuenode *createqueuenode(char *command)
{
    // printf("Creating a node with command %s\n", command);

    Queuenode *newnode = (Queuenode *)malloc(sizeof(Queuenode));
    newnode->command = (char *)malloc(sizeof(strlen(command) * sizeof(char)));
    strcpy(newnode->command, command);

    newnode->nextnode = NULL;

    return newnode;
}

int enqueue(char *command, Queue *que)
{
    // printf("We are enqueuing %s",command);
    if (isfull(que))
        return 0; // deque and then add
    

    if ((que->size != 0) && (strncmp(command, que->rear->command,strlen(command)) == 0))
        return -1; // do nothing

    Queuenode *newnode = createqueuenode(command);

    if (que->size == 0)
    {
        que->head = newnode;
        que->rear = newnode;
        que->size++;
        return 1;
    }

    que->rear->nextnode = newnode;
    que->rear = newnode;
    que->size++;

    return 1;
}

void dequeue(Queue *que)
{
    if (que->size == 0)
        return; // just random error handling

    Queuenode *tobefreed = que->head;
    que->head = que->head->nextnode;
    que->size--;
    free(tobefreed);
}

void history(Queue *que)
{
    int startpos = 0;
    if (que->size > 10)
        startpos = que->size - 10;

    // reach till startpos
    Queuenode *currnode = que->head;
    int i = 0;
    while (i < startpos)
    {
        currnode = currnode->nextnode;
        i++;
    }

    // start printing
    i = 0;
    while ((i < 10) && (currnode != NULL))
    {
        printf("%s", currnode->command);
        if(currnode->command[strlen(currnode->command)-1]!='\n')
            printf("\n");
        currnode = currnode->nextnode;
        i++;
    }
    return;
}

int isfull(Queue *que)
{
    // printf("checking size %d and capacity %d\n",que->size,que->capacity);
    if (que->capacity == que->size)
        return 1;
    else
        return 0;
}

