#include "stomp-tools.h"

#include <string.h>
#include <stdlib.h>

void*
stomp_tools_new(int size)
{
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

char*
stomp_tools_strcpy(const char *src)
{
    char *dest = NULL;
    int len = 0;

    len = strlen(src);

    if (src != NULL) {
        dest = NEW_ARRAY(char, len+1);
        strcpy(dest, src);
    }

    return dest;
}

char*
stomp_tools_strncpy(const char *src, int n)
{
    char *dest = NULL;
    int len = 0;

    len = strlen(src);

    if (src != NULL) {
        if (n > len) {
            dest = NEW_ARRAY(char, len+1);
        } else {
            dest = NEW_ARRAY(char, n+1);
        }
        strncpy(dest, src, n);
    }

    return dest;
}

char*
stomp_tools_strcat(char *first, const char *second)
{
    char *ret = NULL;

    if (first == NULL && second != NULL) {
        ret = stomp_tools_strcpy(second);

    } else if (first != NULL && second == NULL) {
        ret = stomp_tools_strcpy(first);

    } else if (first != NULL && second != NULL) {
        ret = NEW_ARRAY(char, strlen(first) + strlen(second) + 1);

        strcpy(ret, first);
        strcat(ret, second);
    }

    return ret;
}

void
stomp_tools_substr(const char *src, char *dest, int offset, int length)
{
    memcpy(dest, src+offset, length);
}

int
stomp_tools_substr_to(const char *src, char *dest, int offset, char delim)
{
    int length = 0;

    if (src != NULL && (src+offset) != NULL
            && (*(src+offset) != 0x00
                || *(src+offset) != delim)) {

        while ((src+offset+length) != NULL
                && (*(src+offset+length) != 0x00
                    || *(src+offset+length) != delim)) {
            length = length + 1;
        }
    }

    stomp_tools_substr(src, dest, offset, length);

    return offset+length+1;
}