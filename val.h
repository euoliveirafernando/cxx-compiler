#ifndef _val_h_
#define _val_h_

#define uchar8  0x02
#define uint32  0x04
#define uint64  0x08
#define float32 0x10
#define float64 0x20

struct val
{
    u_long type;
    char ** val;
};

typedef struct val val;

#endif