#define BYTE(c) bytecode[info->rip++] = c; ++info->size_code;

#define BYTE1(c1)                     BYTE(c1);
#define BYTE2(c1, c2)                 BYTE(c1); BYTE(c2);
#define BYTE3(c1, c2, c3)             BYTE(c1); BYTE(c2); BYTE(c3);
#define BYTE4(c1, c2, c3, c4)         BYTE(c1); BYTE(c2); BYTE(c3); BYTE(c4);
#define BYTE5(c1, c2, c3, c4, c5)     BYTE(c1); BYTE(c2); BYTE(c3); BYTE(c4); BYTE(c5);
#define BYTE6(c1, c2, c3, c4, c5, c6) BYTE(c1); BYTE(c2); BYTE(c3); BYTE(c4); BYTE(c5); BYTE(c6);

#define ADDRESS(ptr)          *(int*)   (&bytecode[info->rip]) = ptr; info->rip += 4; info->size_code += 4;
#define NUMBER(n)             *(double*)(&bytecode[info->rip]) = n;   info->rip += 8; info->size_code += 8;