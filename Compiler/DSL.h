#define BYTE(c) bytecode[info->rip++] = c; ++info->size_code;

#define BYTE1(c1)                     BYTE(c1);
#define BYTE2(c1, c2)                 BYTE(c1); BYTE(c2);
#define BYTE3(c1, c2, c3)             BYTE(c1); BYTE(c2); BYTE(c3);
#define BYTE4(c1, c2, c3, c4)         BYTE(c1); BYTE(c2); BYTE(c3); BYTE(c4);
#define BYTE5(c1, c2, c3, c4, c5)     BYTE(c1); BYTE(c2); BYTE(c3); BYTE(c4); BYTE(c5);
#define BYTE6(c1, c2, c3, c4, c5, c6) BYTE(c1); BYTE(c2); BYTE(c3); BYTE(c4); BYTE(c5); BYTE(c6);

#define ADDRESS(ptr)          *((int*)   (&(bytecode[info->rip]))) = ptr; info->rip += 4; info->size_code += 4;
#define NUMBER(n)             *(double*)(&bytecode[info->rip]) = n;   info->rip += 8; info->size_code += 8;

#define SAVE_XMM0(n) reg->xmm0_save = 1; reg->xmm0_rip = info->rip; reg->xmm0_rsp = 0; reg->xmm0_count = n;
#define SAVE_XMM1(n) reg->xmm1_save = 1; reg->xmm1_rip = info->rip; reg->xmm1_rsp = 0; reg->xmm1_count = n;

#define CLEAR_XMM0() if (reg->xmm0_save == 1 && reg->xmm0_rsp == 0) reg->xmm0_save = 0; reg->xmm0_rip = 0; reg->xmm0_rsp = 0; reg->xmm0_count = 0;
#define CLEAR_XMM1() if (reg->xmm1_save == 1 && reg->xmm1_rsp == 0) reg->xmm1_save = 0; reg->xmm1_rip = 0; reg->xmm1_rsp = 0; reg->xmm1_count = 0;

#define NOPE_XMM0()  if (reg->xmm0_save == 1)                        \
                     {                                               \
                        for (size_t i = 0; i < reg->xmm0_count; ++i) \
                        {                                            \
                            bytecode[reg->xmm0_rip + i] = 0x90;      \
                        }                                            \
                     }          

#define NOPE_XMM1()  if (reg->xmm1_save == 1)                        \
                     {                                               \
                        for (size_t i = 0; i < reg->xmm1_count; ++i) \
                        {                                            \
                            bytecode[reg->xmm1_rip + i] = 0x90;      \
                        }                                            \
                     }          

#define IF_XMM0(code) if (reg->xmm0_save == 1) {NOPE_XMM0();} else {code}
#define IF_XMM1(code) if (reg->xmm1_save == 1) {NOPE_XMM1();} else {code}

#define XMM0_RSP(n) reg->xmm0_rsp += n;
#define XMM1_RSP(n) reg->xmm1_rsp += n;