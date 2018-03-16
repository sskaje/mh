/*******************************************************

             Test file for Doubly-TAILQ

 ******************************************************/

#include <sys/queue.h>

#include <stdio.h>
#include <stdlib.h>


struct entry {
    int i;
    TAILQ_ENTRY(entry) next;
};

TAILQ_HEAD(top, entry) head;

int main() {

    TAILQ_INIT(&head);
    TAILQ_EMPTY(&head);

    struct entry *n1 = (struct entry*)malloc(sizeof(struct entry));
    n1->i = 1;
    TAILQ_INSERT_HEAD(&head, n1, next);

    struct entry *n2 =(struct entry*) malloc(sizeof(struct entry));
    n2->i = 2;
    TAILQ_INSERT_HEAD(&head, n2, next);

    {
        struct entry *n3 = (struct entry*)malloc(sizeof(struct entry));
        n3->i = 3;
        TAILQ_INSERT_HEAD(&head, n3, next);
    }
    {
        struct entry *n4 = (struct entry*)malloc(sizeof(struct entry));
        n4->i = 4;
        TAILQ_INSERT_HEAD(&head, n4, next);
    }
    {
        struct entry *n5 = (struct entry*)malloc(sizeof(struct entry));
        n5->i = 5;
        TAILQ_INSERT_HEAD(&head, n5, next);
    }
    {
        struct entry *n6 = (struct entry*)malloc(sizeof(struct entry));
        n6->i = 6;
        TAILQ_INSERT_HEAD(&head, n6, next);
    }

    struct entry *np = 0;
    TAILQ_FOREACH(np, &head, next) {
        printf("Hello:%d\n",np->i);
    }
//
//    TAILQ_FOREACH(np, &head, next) {
//        TAILQ_REMOVE(np, next);
//        printf("Delete:%d\n",np->i);
//    }

    while (head.tqh_first != NULL)
    {
        struct entry* temp = head.tqh_first;
        TAILQ_REMOVE(&head, temp, next);
        printf("delete:%d\n", temp->i);
        free(temp);

    }

    return 0;
}


