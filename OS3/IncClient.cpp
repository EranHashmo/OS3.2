//
// Created by owner on 5/25/2020.
//

#include <vector>
#include <map>
#include <utility>
#include <cstdio>
#include <unistd.h>

#include "MapReduceFramework.h"

pthread_mutex_t k2INCResourcesMutex = PTHREAD_MUTEX_INITIALIZER;

class Intgrv: public V1, public V2, public V3
{
public:
    Intgrv(int i)
            : _val(i)
    {}
    void printv()
    {
        printf("val = %d\n", _val);
    }
    int _val;
};

class Intgrk: public K1, public K2, public K3
{
public:
    Intgrk(int i)
            : _val(i)
    {}

    bool operator<(const K1 &other) const override
    {
        return _val < static_cast<const Intgrk&>(other)._val;
    }

    virtual bool operator<(const K2 &other) const
    {
        return _val < static_cast<const Intgrk&>(other)._val;
    }

    virtual bool operator<(const K3 &other) const
    {
        return _val < static_cast<const Intgrk&>(other)._val;
    }
    void printk()
    {
        printf("K = %d\n", _val);
    }
    int _val;

};

class IncClient : public MapReduceClient
{
public:
    std::vector<Intgrk *> *resourcesK2;
    std::vector<Intgrv *> *resourcesV2;

    IncClient()
    {
        resourcesK2 = new std::vector<Intgrk *>;
        resourcesV2 = new std::vector<Intgrv *>;
    }

    ~IncClient()
    {
        while (!resourcesK2->empty())
        {
            delete resourcesK2->at(0);
            resourcesK2->erase(resourcesK2->begin());
        }
        delete resourcesK2;

        while (!resourcesV2->empty())
        {
            delete resourcesV2->at(0);
            resourcesV2->erase(resourcesV2->begin());
        }
        delete resourcesV2;
    }

    void map(const K1* key, const V1* value, void* context) const
    {
        int newK, newv;
        auto* newIntgrk = (Intgrk*)(key);
        auto* newIntgrv = (Intgrv*)(value);
        newK = newIntgrk->_val;
        newv = newIntgrv->_val;
        newv = newv + 1;
        Intgrk* k2 = new Intgrk(newK);
        Intgrv* v2 = new Intgrv(newv);

        pthread_mutex_lock(&k2INCResourcesMutex);
        resourcesK2->push_back(k2);
        resourcesV2->push_back(v2);
        pthread_mutex_unlock(&k2INCResourcesMutex);

//        v2->printv();
        emit2(k2, v2, context);
    }

    void reduce(const K2* key, const std::vector<V2 *> &values, void* context) const
    {
        int res = 0;
        int newK = -1 * ((Intgrk*)key)->_val;
        for (V2* val : values)
        {
            res += static_cast<const Intgrv*>(val)->_val;
        }
        Intgrk* k3 = new Intgrk(newK);
        Intgrv* v3 = new Intgrv(res);

//        v3->printv();
        emit3(k3, v3, context);
    }
};
/*

int main(int argc, char *argv[])
{
        IncClient client;
        InputVec inVec;
        OutputVec outVec;
        Intgrv i1(1);
        Intgrv i2(2);
        Intgrv i3(3);
        inVec.push_back({nullptr, &i1});
        inVec.push_back({nullptr, &i2});
        inVec.push_back({nullptr, &i3});

        startMapReduceJob(client, inVec, outVec, 3 );
        return 0;
}
*/
