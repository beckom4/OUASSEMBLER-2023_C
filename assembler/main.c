#include "assembler.h"
#include <stdio.h>


int main(int argc,char **argv) {
    return assembler_main(argc - 1,argv + 1);
}
