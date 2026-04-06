// bt_demo.c
#include <stdio.h>
#include <string.h>

typedef struct {
    char user[8];
} Session;

void write_audit(Session *s, const char *msg) {
    char buf[16];
    strcpy(buf, msg);           // intentional overflow if msg too long
    printf("audit: %s for %s\n", buf, s->user);
}

void authorize(Session *s, const char *token) {
    if (token[0] == 'X') {
        write_audit(s, "TOKEN_TOO_LONG_FOR_BUF_123456"); // triggers bug
    }
}

void process_request(Session *s, const char *token) {
    authorize(s, token);
}

int main(void) {
    Session s = {.user = "ugonna"};
    process_request(&s, "X-abc");
    return 0;
}
