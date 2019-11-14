#ifndef DES_H
#define DES_H

#define MSB_1_48        0x0000800000000000
#define MSB_1_28        0x0000000008000000
#define LEFTMOST_32     0xFFFFFFFF00000000
#define LEFTMOST_28     0x00FFFFFFF0000000
#define RIGHTMOST_32    0x00000000FFFFFFFF
#define RIGHTMOST_28    0x000000000FFFFFFF

uint64_t des(uint64_t block, uint64_t key, int mode);
void pad_input(char *padded, char *input, int initial_len, int padded_len);

#endif