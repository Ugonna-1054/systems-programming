// bug2.c
#include <stdio.h>
#include <string.h>

int main() {
    char src[] = "hello";
    char dst[4];                 // bug: too small for "hello\0"

    strcpy(dst, src);            // overflow
    size_t len = strlen(dst);    // undefined behavior after overflow

    printf("dst=%s len=%zu\n", dst, len);
    return 0;
}
