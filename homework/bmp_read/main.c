#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BMP_FILE_HDR_SIZE sizeof(struct bmp_file_hdr)
#define DIB_BMP_CORE_HDR_SIZE sizeof(struct dib_bmp_core_hdr)
#define DIB_BMP_INFO_HDR_SIZE sizeof(struct dib_bmp_info_hdr)
#define DIB_HDR_SIZE_SIZE sizeof(dib_hdr_size_t)

enum 
{
    BMP_SIGNATURE_BM = 0x424D,
    BMP_SIGNATURE_BA = 0x4241,
    BMP_SIGNATURE_CI = 0x4349,
    BMP_SIGNATURE_CP = 0x4350,
    BMP_SIGNATURE_IC = 0x4943,
    BMP_SIGNATURE_PT = 0x5054,
};

typedef uint32_t dib_hdr_size_t;

struct __attribute__((packed)) bmp_file_hdr 
{
    uint16_t signature;
    uint32_t file_size;
    uint8_t rsvd[4];
    uint32_t file_offset;
};

_Static_assert(14 == sizeof(struct bmp_file_hdr), "Struct size doesn't match");

static void print_bmp_hdr(struct bmp_file_hdr *hdr)
{   
    printf("BMP File Header contents:\n");
    printf("Header field: 0x0x%02X\n", hdr->signature);
    printf("Size in bytes: %d\n", hdr->file_size);
    printf("Pixel map offset in bytes: %d\n", hdr->file_offset);

    printf("\n\n");
}

struct __attribute__((packed)) dib_bmp_core_hdr 
{
    dib_hdr_size_t hdr_size;
    int16_t width_in_pxl;
    int16_t height_in_pxl;
    uint16_t color_planes;
    uint16_t bits_per_pxl;
};

_Static_assert(12 == sizeof(struct dib_bmp_core_hdr), "Struct size doesn't match");

struct __attribute__((packed)) dib_bmp_info_hdr
{
    dib_hdr_size_t hdr_size;
    uint32_t width_in_pxl;
    uint32_t height_in_pxl;
    uint16_t color_planes;
    uint16_t bits_per_pxl;
    uint32_t compression_method; 
    uint32_t image_size;
    uint32_t h_res;
    uint32_t v_res;
    uint32_t colors_in_pallette;
    uint32_t important_colors;  
};

_Static_assert(40 == sizeof(struct dib_bmp_info_hdr), "Struct size doesn't match");

union dib_hdr
{
    dib_hdr_size_t size;

    struct dib_bmp_core_hdr core_hdr;
    struct dib_bmp_info_hdr info_hdr;

    uint8_t bytes[DIB_BMP_INFO_HDR_SIZE];
};

static void print_dib_core_hdr(struct dib_bmp_core_hdr *hdr)
{
    printf("DIB BMP Core Header contents:\n");
    printf("Header size: %d\n", hdr->hdr_size);
    printf("Width in pixels: %d\n", hdr->width_in_pxl);
    printf("Height in pixels: %d\n", hdr->height_in_pxl);
    printf("Color planes: %d\n", hdr->color_planes);
    printf("Bits per pixel: %d\n", hdr->bits_per_pxl);

    printf("\n\n");
}

static void print_dib_info_hdr(struct dib_bmp_info_hdr *hdr)
{
    printf("DIB BMP Info Header contents:\n");
    printf("Header size: %d\n", hdr->hdr_size);
    printf("Width in pixels: %d\n", hdr->width_in_pxl);
    printf("Height in pixels: %d\n", hdr->height_in_pxl);
    printf("Color planes: %d\n", hdr->color_planes);
    printf("Bits per pixel: %d\n", hdr->bits_per_pxl);

    printf("\n\n");
}

int main(void)
{
    FILE *file;
    struct bmp_file_hdr bmp_hdr;
    struct dib_bmp_core_hdr core_hdr;
    union dib_hdr dib_hdr;

    file = fopen("test24bit.bmp", "rb");

    size_t size = fread(&bmp_hdr, 1, BMP_FILE_HDR_SIZE, file);
    print_bmp_hdr(&bmp_hdr);

    /* 
     * There are many different version of DIB headers.
     * They are recognized by first element indicating header size.
     * So we must read only this element, recognize header version, and read rest of it.
     */
    memset(&dib_hdr, 0, sizeof(dib_hdr));
    size = fread(&dib_hdr, 1, sizeof(dib_hdr_size_t), file);
    switch(dib_hdr.size)
    {
        case DIB_BMP_CORE_HDR_SIZE:
            size = fread((uint8_t*)(&dib_hdr.core_hdr) + DIB_HDR_SIZE_SIZE, 1, DIB_BMP_CORE_HDR_SIZE - DIB_HDR_SIZE_SIZE, file);
            print_dib_core_hdr(&dib_hdr.core_hdr);
            break;
        case DIB_BMP_INFO_HDR_SIZE:
            size = fread((uint8_t*)(&dib_hdr.info_hdr) + DIB_HDR_SIZE_SIZE, 1, DIB_BMP_INFO_HDR_SIZE - DIB_HDR_SIZE_SIZE, file);
            print_dib_info_hdr(&dib_hdr.info_hdr);        
            break;    
    }
    
    fclose(file);

    return 0;
}