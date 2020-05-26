#include <iostream>

#include "MapReduceFramework.h"
#include "MapReduceClient.h"
#include "Barrier.h"
//#include "IncClient.cpp"
#include <map>
#include <iostream>
#include <atomic>
#include <pthread.h>

typedef void* JobHandle;

//enum stage_t {UNDEFINED_STAGE=0, MAP_STAGE=1, SHUFFLE_STAGE=2, REDUCE_STAGE=3};
/*
typedef struct {
        stage_t stage;
        float percentage;
} JobState;
*/
//
struct ThreadContext
{
    int id;
    std::vector<IntermediatePair> *interVec;
};

struct JobContext
{
    const MapReduceClient &_client;
    const InputVec &inVec;
    IntermediateMap *interMap;
    const OutputVec &outVec;
    JobState *_jobState;
    Barrier *_barrier;
    int _multiThreadLvl;
    std::vector<ThreadContext*> tc;
    std::atomic<int> jobAtomic;
    int runningThreadId;

    JobContext(const MapReduceClient &client, const InputVec &inputVec, OutputVec &outputVec, IntermediateMap *intermediateMap, JobState *jobState,
               Barrier *barrier, int multiThreadLevel)
            : _client(client), inVec(inputVec), outVec(outputVec), interMap(intermediateMap), _jobState(jobState),
              _barrier(barrier), _multiThreadLvl(multiThreadLevel), jobAtomic(0), runningThreadId(0)
    {
        tc = std::vector<ThreadContext*>(multiThreadLevel);
    }
};




void emit2 (K2* key, V2* value, void* context)
{
    auto* job = (JobContext*)context;
    IntermediatePair pair(key, value);
    (job)->tc[job->runningThreadId]->interVec->push_back(pair);
}

void emit3 (K3* key, V3* value, void* context)
{
//    outV->emplace_back(OutputPair(key, value));
}

void *startRoutine(void* arg)
{
    auto* job = (JobContext*)arg;
    job->_jobState->stage = MAP_STAGE;
    int oldVal = 0;
    while (true)
    {
        oldVal = (job->jobAtomic)++;
        if (oldVal >= (job->inVec).size())
        {
            job->_jobState->percentage = 100;
            break;
        }
        job->_client.map(job->inVec[oldVal].first, job->inVec[oldVal].second, job);
        job->_jobState->percentage = (float)(oldVal / job->inVec.size()) * 100;
    }

    job->_barrier->barrier();

    //  Shuffle
    job->_jobState->stage = SHUFFLE_STAGE;
    job->_jobState->percentage = 0.0;
    for (auto item: *(job->tc)[job->runningThreadId]->interVec)
    {
        something please

    }

    return 0;
}

JobHandle startMapReduceJob(const MapReduceClient& client, const InputVec& inputVec, OutputVec& outputVec, int multiThreadLevel)
{
    printf("start Map reduce Job\n");
    pthread_t threads[multiThreadLevel];
    ThreadContext tcs[multiThreadLevel];

    auto *jobContext = new JobContext(client, inputVec, outputVec, new IntermediateMap(),
            new JobState({UNDEFINED_STAGE, 0}),
            new Barrier(multiThreadLevel), multiThreadLevel);


    for (int i = 0; i < multiThreadLevel - 1; ++i)
    {
        jobContext->runningThreadId = i;
        pthread_create(threads + i, NULL, startRoutine, jobContext);
    }

    std::vector<V2 *> v2Vec;
//    for (auto item: interMap)
//    {
//        v2Vec.push_back(item.second);
//    }

    client.reduce(nullptr, v2Vec, nullptr);

    return (JobHandle*)jobContext;
}

void waitForJob(JobHandle job);
void getJobState(JobHandle job, JobState* state);
void closeJobHandle(JobHandle job);
