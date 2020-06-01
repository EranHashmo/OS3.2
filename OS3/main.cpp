//
// Created by owner on 5/25/2020.
#include <atomic>
#include "MapReduceClient.h"
#include "MapReduceFramework.h"

//#include "SampleClient.cpp"
#include "Barrier.h"



#include "MapReduceFramework.cpp"
//#include "IncClient.cpp"
#include <iostream>
#include <algorithm>

typedef void* JobHandle;


//int main(int argc, char *argv[])
//{
//    int multiThreadLevel = 5;
//    auto *inVec = new InputVec;
//
//
////    for (int j = 0; j < 3; j++)
////    {
////        for (int i = 3; i < 10; i ++)
////        {
////            Intgrv v(i);
////            Intgrk k(i + 101);
////            InputPair pair;
////            pair.first = new Intgrk(k);
////            pair.second = new Intgrv(v);
////            inVec->push_back(pair);
////        }
////    }
//    for (int i = 0; i < 20; i ++)
//    {
//        Intgrv v(i);
//        Intgrk k(i + 100);
//        InputPair pair;
//        pair.first = new Intgrk(k);
//        pair.second = new Intgrv(v);
//        inVec->push_back(pair);
//    }
//
//
//    auto *outVec = new OutputVec;
//     IncClient client;
//
//    startMapReduceJob(client, *inVec ,*outVec, multiThreadLevel);
//
//    for (std::pair<K3 *, V3 *> oPair: *(std::vector<OutputPair>*)outVec)
//    {
//        printf("%d: %d\n", *(int*)oPair.first, *(int*)oPair.second);
//    }
//
////    std::map<int, int> m;
////    for (int i = 0; i < 10; i++)
////    {
////        m[i] = i+ 100;
////    }
////
//
//    printf("END!\n");
//
//    return 0;
//}
//

