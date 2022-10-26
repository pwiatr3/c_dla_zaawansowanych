#include <stdio.h>
#include <stdint.h>

enum 
{
    BMP_SIGNATURE_BM = 0x424D,
    BMP_SIGNATURE_BA = 0x4241,
    BMP_SIGNATURE_CI = 0x4349,
    BMP_SIGNATURE_CP = 0x4350,
    BMP_SIGNATURE_IC = 0x4943,
    BMP_SIGNATURE_PT = 0x5054,
};

struct __attribute__((packed)) bmp_file_hdr 
{
    uint16_t signature;
    uint32_t file_size;
    uint8_t rsvd[4];
    uint32_t file_offset;
};

int main(void)
{
    FILE *file;
    struct bmp_file_hdr bmp_hdr;
    file = fopen("test24bit.bmp", "rb");
    
    fclose(file);

    return 0;
}