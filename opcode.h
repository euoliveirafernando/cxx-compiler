#ifndef _opcode_h_
#define _opcode_h_

#define cstring         0x01
#define var             0x02
#define val             0x03
#define phi             0x04
#define subscript       0x05
#define ref             0x06
#define neg             0x07
#define not             0x08
#define bitnot          0x09
#define deref           0x0A
#define address_of      0x0B
#define size_of         0x0C
#define mul             0x0D
#define div             0x0E
#define mod             0x0F
#define add             0x10
#define sub             0x11
#define left_shift      0x12
#define right_shift     0x13
#define less            0x14
#define greater         0x15
#define less_equal      0x16
#define greater_equal   0x17
#define equal           0x18
#define not_equal       0x19
#define bitand          0x1A
#define xor             0x1B
#define bitor           0x1C
#define and             0x1D
#define or              0x1E
#define cond            0x1F
#define block           0x20
#define if_then         0x21
#define cgoto           0x22
#define creturn         0x23
#define cload           0x24
#define cstore          0x25

#define is_op(x)        x >= var && x <= cstore

#endif