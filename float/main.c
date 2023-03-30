#include <stdio.h>
#include <float.h>

#ifdef __STDC_IEC_559__
    int floatYes = 1;
#else
    int floatYes = 0;
#endif
#define F0 1e-9
int main() {
    float f = 1e-9;
    float a = F0 - f;
    float b = nextafterf(F0, 0);

    int size = sizeof(f);
    printf("float = %d\n", floatYes);
    return 0;
}