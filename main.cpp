#include "Header.h"
/*
 1.读取所有数据，把有click的找出来
 2.在这87个里面，统计各个维度出现的频率
    例如，对于OS，P(OS_1|click)=(OS_1的个数 ／ 87)
 3.在全部里面，统计各个维度出现的频率
 4.click常数 ＝ 0.0670488
 */
#define OBJID ip_id
#define OBJHASH ip_hash
#define OBJ_STRING OS_info

//#define STAT

#define CLICK 0.0670488

Info *table;//Table of all Info data
const int scale = 9615384;//Number of all records
vector<int> clicked;
long clicked_number=0;
const unsigned mask = 0xFFFF;//used in hash functions
int barrel[mask+1]={0};//used in hash process

//read all data from preprocessed.data
void readAllData();
//register all clicked item in vector clicked
void findAllClicked();

/*The following two function is designed for HASH_VALUE*/
//used with MACRO to handle clicked statistics task
void clicked_handle();
//also used with MACRO to handle statisics in full scale
void stat_handle();

/*The following two function is designed for string/char* */
void clicked_string_handle();
void stat_string_handle();

class Collide_exception : public exception
{
public:
    virtual const char* what() const throw()
    {
        return "Hash Table Collided";
    }
} collide_exception;

/*These two hash function will generate int hashsum according to mask*/
template<class T>
class Hash_16bits
{
public:
    size_t operator () (const T &op) const
    {
        size_t seed = 13131;
        size_t hash = 0;
        char *str = (char *)&op;
        for (int i=0; i<16; i++)
        {
            hash = hash * seed + (*str++);
        }
        return (hash & mask);
    }
};

class Hash_string
{
public:
    size_t operator () (const string &op) const
    {
        size_t seed = 1313131;
        size_t hash = 0;
        const char *str = op.c_str();
        while (*str!=0 && (str - op.c_str())<20 )//防止越界,写法太奇怪
        {
            hash = hash * seed + (*str++);
        }
        return (hash & mask);
    }
};

//Predict the potiential of request_id clicking ads in request_pos
void predict(HASH_VALUE request_id, HASH_VALUE request_pos)
{
    //选定用户,根据用户生成用户向量
    //我偷懒，就选第一条，但是这个是一个可以做大量工作的地方
    Info selected_vec;
    for (int i=0; i<scale; i++)
    {
        if (table[i].id == request_id)
        {
            selected_vec = table[i];
        }
    }
    //选定监测点
    selected_vec.pos_id = request_pos;
    //生成完整向量
    //读入所有生成的概率表
    string forms[]={
        "cl_id","cl_pos","cl_ads","cl_ip","cl"
    };
    typedef HASH_VALUE* pHash_Value;
    pHash_Value requests[4]={
        &selected_vec.id,&selected_vec.pos_id,&selected_vec.ads_id,&selected_vec.ip_id
    };
    double P[4]={0};
    for (int i=0; i<4; i++)
    {
        fstream fin(forms[i]+".txt");
        string hashid;
        double potiential;
        while (fin>>hashid>>potiential)
        {
            if (HASH_VALUE(hashid) == *(requests[i]))
            {
                P[i] = potiential;
                fin.close();
                break;
            }
        }
        if(P[i]==0) P[i]=1e-7;//A trick
        fin.close();
    }
    
    string stats[]={
        "id_stat","ip_stat","ads_stat","pos_stat"
    };
    double Q[4]={0};
    for (int i=0; i<4; i++)
    {
        fstream fin(stats[i]+".txt");
        string hashid;
        double potiential;
        while (fin>>hashid>>potiential)
        {
            if (HASH_VALUE(hashid) == *(requests[i]))
            {
                Q[i] = potiential;
                fin.close();
                break;
            }
        }
        if(Q[i]==0) Q[i]=1e-7;//A trick
        fin.close();
    }
    //计算概率
    double final_p = CLICK * P[0]/Q[0] * P[1]/Q[1] * P[2]/Q[2] * P[3]/Q[3];
    printf("%.4lf\n",final_p);
}

int main()
{
    readAllData();
#ifdef STAT
    stat_handle();
#else
    findAllClicked();
    stat_string_handle();
#endif
    //predict(HASH_VALUE("ZjkxMzg5OGYwNjQz"), HASH_VALUE("MzExZjI3YjY3Njdh"));
//    for (int i=0; i<100; i++)
//    {
//        predict(table[i].id, table[i].pos_id);
//    }
    delete []table;
    return 0;
}



void readAllData()
{
    table = new Info[scale];
    fstream fin("preprocessed.data");
    fin.read((char *)table, scale*sizeof(Info));
    fin.close();
}

void findAllClicked()
{
    for (int i=0; i<scale; i++)
    {
        if (table[i].click==1)
        {
            clicked.push_back(i);
        }
    }
    clicked_number = clicked.size();
    
}

///////////////////////////////////////////////////////////////////////////
void stat_handle()
{
    using namespace __gnu_cxx;
    hash_map<HASH_VALUE, HASH_VALUE, Hash_16bits<HASH_VALUE> > OBJHASH;
    for (int i=0; i<scale; i++)
    {
        if (OBJHASH.count(table[i].pos_id)!=0)
        {
            if( !(OBJHASH[table[i].pos_id] == table[i].OBJID) )
            {
                cout<<"collide at "<<i<<endl;
                return;
            }
            barrel[ Hash_16bits<HASH_VALUE>()(table[i].OBJID) ]++;
        }
        else
        {
            OBJHASH.insert(pair<HASH_VALUE, HASH_VALUE>(table[i].OBJID, table[i].OBJID));
            barrel[ Hash_16bits<HASH_VALUE>()(table[i].OBJID) ]++;
        }
    }
    unsigned int sum=0;
    for (int i=0; i<mask+1; i++)
    {
        sum+=barrel[i];
    }
    hash_map<HASH_VALUE, HASH_VALUE, Hash_16bits<HASH_VALUE> >::iterator it;
    for (it = OBJHASH.begin(); it != OBJHASH.end(); it++)
    {
        cout<<(*it).second;
        printf("\t%.12lf\n",(double)barrel[Hash_16bits<HASH_VALUE>()((*it).first)]/sum);
    }
}

void clicked_handle()
{
    using namespace __gnu_cxx;
    hash_map<HASH_VALUE, HASH_VALUE, Hash_16bits<HASH_VALUE> > OBJHASH;
    for (int i=0; i<clicked_number; i++)
    {
        int selected_item = clicked[i];
        if (OBJHASH.count(table[selected_item].OBJID)!=0)
        {
            if( !(OBJHASH[table[selected_item].OBJID] == table[selected_item].OBJID) )
            {
                cout<<"collide at "<<i<<endl;
                return;
            }
            barrel[ Hash_16bits<HASH_VALUE>()(table[selected_item].OBJID) ]++;
        }
        else
        {
            OBJHASH.insert(pair<HASH_VALUE, HASH_VALUE>(table[selected_item].OBJID, table[selected_item].OBJID));
            barrel[ Hash_16bits<HASH_VALUE>()(table[selected_item].OBJID) ]++;
        }
    }
    unsigned int sum=0;
    for (int i=0; i<mask+1; i++)
    {
        sum+=barrel[i];
    }
    hash_map<HASH_VALUE, HASH_VALUE, Hash_16bits<HASH_VALUE> >::iterator it;
    for (it = OBJHASH.begin(); it != OBJHASH.end(); it++)
    {
        cout<<(*it).second;
        printf("\t%.12lf\n",(double)barrel[Hash_16bits<HASH_VALUE>()((*it).first)]/sum);
    }
}
///////////////////////////////////////////////////////////////////////////

void clicked_string_handle()
{
    //找出所有的clicked
    //检查存在性
    //在hash表里面注册
    //在barrel里面注册
    
    
    //hash表
    using namespace __gnu_cxx;
    hash_map<string, string, Hash_string> hashMap;
    //所有的clicked
    for (int i=0; i<clicked_number; i++)
    {
        //选出来的Info
        Info selected  = table[clicked[i]];
        //检查存在性
        if (hashMap.count(selected.OBJ_STRING) == 0)//不存在
        {
            //在hash表中注册
            hashMap.insert(pair<string, string>(selected.OBJ_STRING, selected.OBJ_STRING));
        }
        else //检查碰撞
            if (hashMap[selected.OBJ_STRING] != selected.OBJ_STRING)
            {
                throw collide_exception;
            }
        //在barrel中注册
        barrel[ Hash_string()(selected.OBJ_STRING) ]++;
    }
    //输出概率
    hash_map<string, string, Hash_string>::iterator it = hashMap.begin();
    for (; it != hashMap.end(); it++)
    {
        printf("%s\t%.12lf\n",it->second.c_str(), (double)barrel[ Hash_string()(it->first) ]/clicked_number);
    }
}

void stat_string_handle()
{
    //检查存在性
    //在hash表里面注册
    //在barrel里面注册
    
    
    //hash表
    using namespace __gnu_cxx;
    hash_map<string, string, Hash_string> hashMap;
    //所有
    for (int i=0; i<scale; i++)
    {
        //选出来的Info
        Info selected  = table[i];
        //检查存在性
        if (hashMap.count(selected.OBJ_STRING) == 0)//不存在
        {
            //在hash表中注册
            hashMap.insert(pair<string, string>(selected.OBJ_STRING, selected.OBJ_STRING));
        }
        else //检查碰撞
            if (hashMap[selected.OBJ_STRING] != selected.OBJ_STRING)
            {
                throw collide_exception;
            }
        //在barrel中注册
        barrel[ Hash_string()(selected.OBJ_STRING) ]++;
    }
    //输出概率
    hash_map<string, string, Hash_string>::iterator it = hashMap.begin();
    for (; it != hashMap.end(); it++)
    {
        printf("%s\t%.12lf\n",it->second.c_str(), (double)barrel[ Hash_string()(it->first) ]/scale);
    }
}

