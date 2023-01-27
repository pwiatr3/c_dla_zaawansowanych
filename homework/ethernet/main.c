#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#define TAG_POSITION_BYTE 13 // Position of the first byte of frame type

// MAKRA OPERACJI BITOWYCH 
#define MULTI_MASK(len) ((1U << (len)) - 1U)
#define MULTI_MASK_SHIFTED(start, len) (MULTI_MASK(len) << start)
#define MULTI_READ(in_val, start, len) ((in_val & MULTI_MASK_SHIFTED(start, len)) >> start)
#define MULTI_CLEAR(in_val, start, len) (in_val & ~MULTI_MASK_SHIFTED(start, len))

#define MAC_DESTINATION_OFFSET 0 // byte number where destination mac starts
#define MAC_SOURCE_OFFSET 6 // byte number where destination mac starts

typedef enum 
{
    IPv4 = 0x0800,
    ARP = 0x0806,
    IPv6 = 0x86DD,
    IEEE802_1Q = 0x8100,
    IEEE802_1AD = 0x88a8 
} ether_type;

typedef enum
{
    WRONG_LENGTH = 0,
    CRC_ERROR
} frame_error;
typedef union 
{
    uint32_t hdr;
    uint8_t bytes[4];
} ieee802_1Q_hdr;

typedef struct 
{
    uint8_t bytes[6];
} mac_address;

typedef union 
{
    uint32_t all;
    uint8_t bytes[4];
} ipv4_address;

typedef struct __attribute__((packed)) 
{
    uint8_t destination_mac[6];
    uint8_t source_mac[6];
    uint8_t length_type[2];
} eth_header;

_Static_assert(14 == sizeof(eth_header), "Struct size doesn't match");

typedef struct __attribute__((packed))
{
    uint8_t destination_mac[6];
    uint8_t source_mac[6];
    ieee802_1Q_hdr tag;
    uint16_t length_type;
} eth_header_with_tag;

_Static_assert(18 == sizeof(eth_header_with_tag), "Struct size doesn't match");

typedef struct 
{
    uint8_t version;
    uint8_t ihl; // Internet Header Length
    uint8_t dscp; // Differentiated Services Code Point
    uint8_t ecn; // Explicit Congestion Notification
    uint16_t total_lenght;
    uint16_t identification;
    uint8_t flags;
    uint16_t fragment_offset;
    uint8_t time_to_live;
    uint8_t protocol;
    uint16_t header_crc;
    ipv4_address source_ip_addr;
    ipv4_address destination_ip_addr;
    uint8_t options[];
} ipv4_header;

// Function prototypes
size_t read_frame_size(const char* path);
void load_bytes_to_buffer(const char* path, uint8_t* buf);
void print_buffer(uint8_t* buf, size_t num);
ether_type check_ethernet_frame_type();
eth_header read_eth_header();
ipv4_header read_ipv4_header(uint8_t* bytes);
void print_mac(char* text, mac_address mac);
void printf_ip_addr(char* text, ipv4_address ipv4_addr);
void print_eth_frame_info(eth_header *hdr);
int is_big_endian(void);
mac_address get_mac(uint8_t* buf, size_t offset);
uint16_t get_eth_length_type(uint8_t* buf);
char* ip_protocol_name(uint8_t protocol_number);
void print_ipv4_header(ipv4_header hdr);


int main(void)
{   
    assert(is_big_endian() == false); // check if machine uses little endian
    const char* path = "frame_raw.txt";
    size_t frame_size = read_frame_size(path);
    uint8_t frame_bytes[frame_size];
    load_bytes_to_buffer(path, frame_bytes);
    print_buffer(frame_bytes, frame_size);
    mac_address destination_mac = get_mac(frame_bytes, MAC_DESTINATION_OFFSET);
    mac_address source_mac = get_mac(frame_bytes, MAC_SOURCE_OFFSET);
    print_mac("destination mac: ", destination_mac);
    print_mac("source mac: ", source_mac);
    uint16_t frame_length_type = get_eth_length_type(frame_bytes);
    printf("Eth type is: %04x\n", frame_length_type);
    ipv4_header ipv4_hdr;
    if(frame_length_type > 46 && frame_length_type < 1500)
    {

    }
    else
    {
        switch(frame_length_type)
        {
            case IPv4:
                ipv4_hdr = read_ipv4_header(frame_bytes);
                break;
            case ARP:
                break;
            case IPv6:
                break;
            case IEEE802_1Q:
                break;   
            case IEEE802_1AD:
                break;                             
        }
    }

    char* protocol_name = ip_protocol_name(ipv4_hdr.protocol);
    printf("protocol_name: %s\n", protocol_name);
    print_ipv4_header(ipv4_hdr);

    return 0;
}

size_t read_frame_size(const char* path)
{
    FILE *file;
    ether_type type = IPv4;
    size_t byte_num = 0;
    uint16_t byte;
    char str[10];
    file = fopen(path, "r");

    while (fscanf(file, "%2hx", &byte) == 1) 
    {
        byte_num++; 
    }
    // fseek(file, 0, SEEK_SET);
    rewind(file);
    fclose(file);
    return byte_num;
}

void load_bytes_to_buffer(const char* path, uint8_t* buf)
{
    FILE *file;
    ether_type type = IPv4;
    int byte_num = 0;
    uint16_t byte = 0;
    file = fopen(path, "r");

    while (fscanf(file, " %2hx", &byte) == 1) 
    {
        buf[byte_num++] = byte;
    }
    // fseek(file, 0, SEEK_SET);
    fclose(file);
}

void print_buffer(uint8_t* buf, size_t num)
{
    int i = 0;
    for(i = 0; i < num; i++)
    {
        printf("byte[%d] = %02x\n", i, buf[i]);
    }
}

ipv4_header read_ipv4_header(uint8_t* bytes)
{
    ipv4_header hdr;
    size_t offset = 14;
    bytes += offset; 
    hdr.version = MULTI_READ(bytes[0], 4, 4);
    hdr.ihl = MULTI_READ(bytes[0], 0, 4);
    hdr.dscp = MULTI_READ(bytes[1], 2, 6);
    hdr.ecn = MULTI_READ(bytes[1], 0, 2);
    hdr.total_lenght = (bytes[2] << 8) + bytes[3];

    hdr.identification = (bytes[4] << 8) + bytes[5];
    hdr.flags = MULTI_READ(bytes[6], 5, 3);
    hdr.fragment_offset =  MULTI_READ(bytes[6], 0, 5) << 8 + bytes[7];
    hdr.time_to_live = bytes[8];
    hdr.protocol = bytes[9];
    hdr.header_crc = (bytes[10] << 8) + bytes[11];
    hdr.source_ip_addr.all = (bytes[12] << 24) + (bytes[13] << 16) + (bytes[14] << 8) + bytes[15];
    hdr.destination_ip_addr.all = (bytes[16] << 24) + (bytes[17] << 16) + (bytes[18] << 8) + bytes[19];
    return hdr;
}

void print_mac(char* text, mac_address mac)
{
    int i = 0;
    uint8_t byte = 0;
    printf("%s", text);
    for(i = 0; i < 5; i++)
    {
        byte = mac.bytes[i];
        printf("%02x:", byte);
    }
    byte = mac.bytes[i];
    printf("%02x\n", byte);

}

void printf_ip_addr(char* text, ipv4_address ipv4_addr)
{
    int i = 0;
    uint8_t byte = 0;
    printf("%s", text);
    if(is_big_endian())
    {
        for(i = 0; i < 4; i++)
        {
            byte = ipv4_addr.bytes[i];
            printf("%d:", byte);
        }
        byte = ipv4_addr.bytes[i];
        printf("%d\n", byte);
    }
    else
    {
        for(i = 3; i > 0; i--)
        {
            byte = ipv4_addr.bytes[i];
            printf("%d:", byte);
        }
        byte = ipv4_addr.bytes[i];
        printf("%d\n", byte);       
    }
}

void print_eth_frame_info(eth_header *hdr)
{
    
}

int is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1;
}

mac_address get_mac(uint8_t* buf, size_t offset)
{
    mac_address dst_mac;
    int i = 0;
    for(i = 0; i < 6; i++)
    {
        dst_mac.bytes[i] = buf[i + offset];
    }

    return dst_mac;   
}

uint16_t get_eth_length_type(uint8_t* buf)
{
    return (buf[12] << 8) + buf[13]; 
}

char* ip_protocol_name(uint8_t protocol_number)
{
    const char* path = "ip_protocols_numbers.txt";
    const size_t number_of_protocol_names = 144;
    const size_t protocol_names_max_size = 15;
    FILE* file;
    file = fopen(path, "r");
    char protocol_names[number_of_protocol_names][protocol_names_max_size];
    memset(protocol_names, ' ', number_of_protocol_names * protocol_names_max_size);
    size_t protocol_index = 0;
    char str[30]; 
    
    while(fscanf(file, "%s", str) == 1)
    {
        // printf("str = %s\n", str);
        if(isalpha(str[0]))
        {
            strncpy(protocol_names[protocol_index], str, protocol_names_max_size);      
        }
        if(isdigit(str[0]))
        {
            protocol_index = atoi(str);
        }
        int j = 0;
    }
    char* n = protocol_names[protocol_number];
    return  n;
}

void print_ipv4_header(ipv4_header hdr)
{
    printf("Internet Protocol Version 4:\n");
    printf("Version: %d\n", hdr.version);
    printf("Header Length: %d bytes\n", hdr.ihl * 8);
    printf("Differentianed Services Codepoint: %d\n", hdr.dscp );
    printf("Explicit Congestion Notification: %d\n", hdr.ecn);
    printf("Total Length: %d\n", hdr.total_lenght);
    printf("Identification: 0x%x\n", hdr.identification);
    printf("Flags: 0x%x\n", hdr.flags);
    printf("Fragment Offset: %d\n", hdr.fragment_offset);
    printf("Time to Live: %d\n", hdr.time_to_live);
    printf("Protocol: %s\n",ip_protocol_name(hdr.protocol));
    printf("Header Checksum: 0x%x\n", hdr.header_crc);
    printf_ip_addr("Source address:", hdr.source_ip_addr);
    printf_ip_addr("Destination address:", hdr.destination_ip_addr);
}