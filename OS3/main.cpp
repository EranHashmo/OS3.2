//
// Created by owner on 5/25/2020.
#include "MapReduceFramework.h"
#include "MapReduceClient.h"
#include "IncClient.cpp"


int main(int argc, char *argv[])
{
    IncClient client;
    InputVec inVec;
    OutputVec outVec;
    for (int i = 0; i < 20; i++)
    {
        auto *j = new Intgrv(i);
        inVec.push_back({nullptr, j});
    }

    startMapReduceJob(client, inVec, outVec, 3 );

    printf("values: \n");
    for (auto item : outVec)
    {
        printf("%d\t", ((Intgrv*)(item.second))->_val);
    }

    return 0;
}
//

