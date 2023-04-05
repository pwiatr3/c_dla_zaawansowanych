// #include "module1.h"
#include <stdio.h>

// #define DEBUG
#ifdef DEBUG
#define LOG(text) log(text)
#else
#define LOG(text)
#endif

// #if __STDC_VERSION__ > 0
// #error "error"
// #endif
__attribute__((optimize("01")))
void log(char * text) 
{
    printf("%s\n", text);
}
int main(void)
{
    // printf("%ld\n", __STDC_VERSION__);
    // LOG("loguje");
    printf("Compiled on %s, %s\n", __DATE__, __TIME__);
    printf("Error in file: %s, Function: %s, Line: %d\n", __FILE__, __func__, __LINE__);
    printf("GNUC %d\n", __GNUC__);
    printf("COUNTER %d\n", __COUNTER__);
    printf("COUNTER %d\n", __COUNTER__);

    return 0;
}