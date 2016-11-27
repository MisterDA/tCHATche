#include <stdlib.h>
#include <stdio.h>
#include "tchatche.h"

int main()
{
    char *path = mktmpfifo_client();
    printf("%s\n", path);
    free(path);
    return 0;
}
