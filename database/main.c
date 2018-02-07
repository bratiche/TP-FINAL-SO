#include "db_functions.h"
#include <sqlite3.h>
#include <stdio.h>

int main(int argc, char const *argv[]){
    printf("%s\n",sqlite3_libversion());
    book("Shrek",2,4,"tobias",19);
    cancel("Shrek",2,4,"tobias",19);
    return 0;
}