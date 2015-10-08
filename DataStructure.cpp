#include "Header.h"

Info *seq;

void searchForFirstUser()
{
    vector<int> positions;
    fstream fin("preprocessed.data");
    seq = new Info[num];
    fin.read((char *)seq, sizeof(Info)*num);
    HASH_VALUE firstID =seq[0].id;
    for (int i=1; i<num; i++)
    {
        if (seq[i].id == firstID)
        {
            positions.push_back(i);
        }
    }
    for (int i=0; i<positions.size(); i++)
    {
        cout<<i<<' ';
    }
    cout<<endl;
    delete []seq;
    fin.close();
}

void searchForClicks()
{
    vector<int> positions;
    fstream fin("preprocessed.data");
    seq = new Info[num];
    fin.read((char *)seq, sizeof(Info)*num);
    for (int i=1; i<num; i++)
    {
        if (seq[i].click == 1)
        {
            positions.push_back(i);
        }
    }
    for (int i=0; i<positions.size(); i++)
    {
        cout<<positions[i]<<'\t'<<seq[positions[i]].pos_id<<endl;
    }
    cout<<positions.size()<<endl;
    fin.close();
    delete []seq;
}