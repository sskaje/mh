/*******************************************************

             Test file for Doubly-List

 ******************************************************/

#include <sys/queue.h>

#include <stdio.h>
#include <stdlib.h>


struct entry {
    int i;
    LIST_ENTRY(entry) next;
};

LIST_HEAD(top, entry) head;

int main() {

    LIST_INIT(&head);
    LIST_EMPTY(&head);

    struct entry *n1 = (struct entry*)malloc(sizeof(struct entry));
    n1->i = 1;
    LIST_INSERT_HEAD(&head, n1, next);

    struct entry *n2 =(struct entry*) malloc(sizeof(struct entry));
    n2->i = 2;
    LIST_INSERT_HEAD(&head, n2, next);

    {
        struct entry *n3 = (struct entry*)malloc(sizeof(struct entry));
        n3->i = 3;
        LIST_INSERT_HEAD(&head, n3, next);
    }
    {
        struct entry *n4 = (struct entry*)malloc(sizeof(struct entry));
        n4->i = 4;
        LIST_INSERT_HEAD(&head, n4, next);
    }
    {
        struct entry *n5 = (struct entry*)malloc(sizeof(struct entry));
        n5->i = 5;
        LIST_INSERT_HEAD(&head, n5, next);
    }
    {
        struct entry *n6 = (struct entry*)malloc(sizeof(struct entry));
        n6->i = 6;
        LIST_INSERT_HEAD(&head, n6, next);
    }

    struct entry *np = 0;
    LIST_FOREACH(np, &head, next) {
        printf("Hello:%d\n",np->i);
    }
//
//    LIST_FOREACH(np, &head, next) {
//        LIST_REMOVE(np, next);
//        printf("Delete:%d\n",np->i);
//    }

    while (head.lh_first != NULL)
    {
        struct entry* temp = head.lh_first;
        LIST_REMOVE(temp, next);
        printf("delete:%d\n", temp->i);
        free(temp);

    }

    return 0;
}


