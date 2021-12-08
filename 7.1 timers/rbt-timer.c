#include <stdio.h>
#include <string.h>
#include "rbt-timer.h"

void print_hello(timer_entry_t *te) {
    printf("hello world time = %u\n", te->timer.key);
}

int main()
{
    init_timer();
    timer_entry_t *te = malloc(sizeof(timer_entry_t));
    memset(te, 0, sizeof(timer_entry_t));
    te->handler = print_hello;
    add_timer(te, 3000);
    for (;;) {
        expire_timer();
        usleep(10000);
    }
    return 0;
}