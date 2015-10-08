//
//  Header.h
//  Classifier0
//
//  Created by Michael on 15/10/4.
//  Copyright © 2015年 Michael. All rights reserved.
//

#if defined __GNUC__ || defined __APPLE__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <exception>
#define HASHLEN 17
using namespace std;
const int line = 120;
const long num = 10000;
const long cache_size = line * num;
struct HASH_VALUE
{
    //char bytes[HASHLEN];
    long long seg1,seg2;
    HASH_VALUE (){}
    HASH_VALUE (string &str)//不安全
    {
        char *dist = (char *)this;
        for(int i=0; i<16; i++)
        {
            dist[i] = str[i];
        }
    }
    HASH_VALUE (const char str[])//不安全
    {
        char *dist = (char *)this;
        for(int i=0; i<16; i++)
        {
            dist[i] = str[i];
        }
    }
};
bool operator == (const HASH_VALUE &op1, const HASH_VALUE &op2);
ostream &operator << (ostream &out, const HASH_VALUE &op);

struct Info
{
    HASH_VALUE id, ads_id, pos_id, ip_id;//16bytes HASH value
    int lang;//en OR zh , 0->zh, 1->en
    char OS_info[HASHLEN];//whose length is unknown
    long timeStamp;//Unix Time stamp
    int stable, click;//is stable / is clicked
};


bool operator == (const Info & op1, const Info & op2);
void searchForFirstUser();
void searchForClicks();
