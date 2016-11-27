#include <stdlib.h>
#include <stdio.h>
#include "tchatche.h"

int main()
{
    char *path = mktmpfifo_server();
    printf("%s\n", path);
    free(path);
    return 0;
}
