#include "smartarr/defines.h"
#include "smartarr/third/list.h"

#include "third/greatest.h"

typedef struct mylist {
    list_head_t list;
    int id;
} mylist_t;

TEST test_empty(void)
{
    constexpr unsigned int SIZE = 16;
    mylist_t items[SIZE];

    LIST_HEAD(list);
    ASSERT(list_empty(&list));

    items[0].id = 0;
    list_add_tail(&items[0].list, &list);
    ASSERT(!list_empty(&list));
    ASSERT(list_is_singular(&list));

    list_del(&items[0].list);
    ASSERT(list_empty(&list));
    list_add_tail(&items[0].list, &list);
    ASSERT(!list_empty(&list));

    mylist_t* p = list_entry(&items[0].list, mylist_t, list);
    //printf("%p id:%d\n", p, p->id);
    ASSERT_EQ(0, p->id);

    PASS();
}

TEST test_add_tail(void)
{
    constexpr unsigned int SIZE = 16;
    mylist_t items[SIZE];

    LIST_HEAD(list);
    ASSERT(list_empty(&list));

    items[0].id = 0;
    list_add_tail(&items[0].list, &list);

    mylist_t* p = list_entry(&items[0].list, mylist_t, list);
    //printf("%p id:%d\n", p, p->id);
    ASSERT_EQ(0, p->id);

    items[1].id = 1;
    list_add_tail(&items[1].list, &list);
    p = list_entry(&items[1].list, mylist_t, list);
    ASSERT_EQ(1, p->id);

    items[2].id = 2;
    list_add_tail(&items[2].list, &list);
    p = list_entry(&items[2].list, mylist_t, list);
    ASSERT_EQ(2, p->id);
    
    ASSERT_EQ(3, list_count_nodes(&list));

    int i = 0;
    for (list_head_t* ptr = list.next; ptr != &list; ptr = ptr->next) {
        p = list_entry(ptr, mylist_t, list);
        //printf("id:%d\n", p->id);
        ASSERT_EQ(i, p->id);
        ++i;
    }

    PASS();
}


SUITE(operations) {
    RUN_TEST(test_empty);
    RUN_TEST(test_add_tail);
}

GREATEST_MAIN_DEFS();

int main(int argc UNUSED, char **argv UNUSED) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(operations);

    GREATEST_MAIN_END();
}
