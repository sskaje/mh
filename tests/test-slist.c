/*******************************************************

             Test file for Singly-List

 ******************************************************/

#include <sys/queue.h>

#include <stdio.h>
#include <stdlib.h>


struct entry {
    int i;
    SLIST_ENTRY(entry) next;
};

SLIST_HEAD(top, entry) head;

int main() {

    SLIST_INIT(&head);
    SLIST_EMPTY(&head);

    struct entry *n1 = (struct entry*)malloc(sizeof(struct entry));
    n1->i = 1;
    SLIST_INSERT_HEAD(&head, n1, next);

    struct entry *n2 =(struct entry*) malloc(sizeof(struct entry));
    n2->i = 2;
    SLIST_INSERT_HEAD(&head, n2, next);

    {
        struct entry *n3 = (struct entry*)malloc(sizeof(struct entry));
        n3->i = 3;
        SLIST_INSERT_HEAD(&head, n3, next);
    }
    {
        struct entry *n4 = (struct entry*)malloc(sizeof(struct entry));
        n4->i = 4;
        SLIST_INSERT_HEAD(&head, n4, next);
    }
    {
        struct entry *n5 = (struct entry*)malloc(sizeof(struct entry));
        n5->i = 5;
        SLIST_INSERT_HEAD(&head, n5, next);
    }
    {
        struct entry *n6 = (struct entry*)malloc(sizeof(struct entry));
        n6->i = 6;
        SLIST_INSERT_HEAD(&head, n6, next);
    }

    struct entry *np = 0;
    SLIST_FOREACH(np, &head, next) {
        printf("Hello:%d\n",np->i);
    }

    while (head.slh_first != NULL)
    {
        struct entry* temp = head.slh_first;
        SLIST_REMOVE_HEAD(&head, next);
        printf("delete:%d\n", temp->i);
        free(temp);

    }

    return 0;
}


