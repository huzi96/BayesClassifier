#include "Header.h"

ostream &operator << (ostream &out, const HASH_VALUE &op)
{
    char *s = (char *)&op;
    for (int i=0; i<16; i++)
    {
        out<<s[i];
    }
    return out;
}

bool operator == (const HASH_VALUE &op1, const HASH_VALUE &op2)
{
    return (op1.seg1==op2.seg1 && op1.seg2 == op2.seg2);
}

bool operator == (const Info & op1, const Info & op2)
{
    char *p1 = (char *)&op1;
    char *p2 = (char *)&op2;
    for (int i=0; i<sizeof(Info); i++)
    {
        if (p1[i]!=p2[i])
        {
            return false;
        }
    }
    return true;
}

