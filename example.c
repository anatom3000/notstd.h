#include "notstd.h"

typedef_Vec(int, IntVec);
typedef_Stack(int, IntStack);
typedef_List(int, IntList);
typedef_Queue(int, IntQueue);

IntList IntList_rev(move IntList self, IntList acc) {
    List_match(self,
        {
            return acc;
        },
        h, t, {
            free(self);
            return IntList_rev(t, List_Cons(h, acc));
        }
    );
}

void IntList_print(IntList self) {
    List_match(self,
        {},
        h, t, {
            println("%d", h);
            IntList_print(t);
        }
    );
}

int list() {
    IntList lst = NULL;
    lst = List_Cons(1, lst);
    lst = List_Cons(2, lst);
    lst = List_Cons(3, lst);
    
    IntList_print(lst);
    IntList reversed = IntList_rev(lst, NULL);
    IntList_print(reversed);

    return 0;
}

int vec() {
    IntVec v = {0};

    for (int i = 0; i < 10; i++) {
        Vec_append(&v, i);
    }

    for (int i = 0; i < v.len; i++) {
        dbg("%d", i);
    }

    free(v.items);

    return 0;
}

int queue() {
    IntQueue q = {0};

    for (int i = 0; i < 10; i++) {
        Queue_push(&q, i);
    }
    
    while (true) {
        int* x = Queue_pop(&q);
        if (x == NULL) break;

        dbg("%d", *x);
    }

    free(q.items);

    return 0;
}

int hashmap() {
    return 0;
}

int main() {
}
