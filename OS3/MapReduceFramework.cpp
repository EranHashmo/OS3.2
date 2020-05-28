
#include "MapReduceFramework.h"
#include "MapReduceClient.h"
#include "Barrier.h"
//#include "IncClient.cpp"
#include <map>
#include <iostream>
#include <atomic>
#include <pthread.h>
#include <algorithm>

typedef void* JobHandle;


struct ThreadContext
{
    int id;
    void * c;
};

struct JobContext
{
//    pthread_t *threads;
//    pthread_t shuffleThread;
    const MapReduceClient &_client;
    const InputVec &inVec;
    IntermediateMap *interMap;
    const OutputVec &outVec;
    JobState *_jobState;
    Barrier *_barrier;
    int _multiThreadLvl;
//    std::map<pthread_t*, ThreadContext*> tc;
    std::vector<ThreadContext*> tcs;
    std::atomic<int> jobAtomic;
//    std::vector<int> finishedThreads;
    std::atomic<int> finishedThreads;
    std::atomic<int> runningThreadId;

    JobContext(const MapReduceClient &client, const InputVec &inputVec, OutputVec &outputVec, IntermediateMap *intermediateMap, JobState *jobState,
               Barrier *barrier, int multiThreadLevel)
            : _client(client), inVec(inputVec), outVec(outputVec), interMap(intermediateMap), _jobState(jobState),
              _barrier(barrier), _multiThreadLvl(multiThreadLevel), jobAtomic(0), runningThreadId(0)
    {
//        tc = std::vector<ThreadContext*>(multiThreadLevel);
//        tc = std::map<pthread_t*, ThreadContext*>();
    }
};

bool key2Comp(IntermediatePair p1, IntermediatePair p2)
{
    return *p1.first < *p2.first;
}


void emit2 (K2* key, V2* value, void* context)
{
//    auto* job = (JobContext*)context;
    IntermediatePair pair(key, value);
//    (job)->tc[job->runningThreadId]->interVec->push_back(pair);
//    (job)->tc[pthread_self()]->interVec->push_back(pair);
    auto *tc = (ThreadContext*)context;
    ((std::vector<IntermediatePair> *)tc->c)->push_back(pair);
}

void emit3 (K3* key, V3* value, void* context)
{
//    outV->emplace_back(OutputPair(key, value));
}

void *startRoutine(void* arg)
{

    auto* job = (JobContext*)arg;
    job->_jobState->stage = MAP_STAGE;

    auto *threadVector = new std::vector<IntermediatePair>;
    auto *tc = new ThreadContext{job->runningThreadId++, threadVector};


    job->tcs.push_back(tc);
    int oldVal = 0;
//    if (job->runningThreadId == 0)
    if (tc->id == 0)
    {
        int id = 1;
        while(job->finishedThreads != job->_multiThreadLvl and job->_jobState->percentage != 100)
        {
            for (auto pair : *(std::vector<IntermediatePair>*)job->tcs[id]->c)
            {

                if (job->interMap->find(pair.first) == job->interMap->end())
                {
                    std::vector <V2 *> vector;
                    (*job->interMap)[pair.first] = vector;
                }
                (*job->interMap)[pair.first].push_back(pair.second);
            }
            id = (id + 1) % job->_multiThreadLvl;
            if (id == 0)
            {
                id = (id + 1)% job->_multiThreadLvl;
            }

//            for (auto item: *(job->tcs)[job->runningThreadId]->interVec)
//            {
//                // maybe it would be better in some other function? so this one will be smaller.
//                // todo add something about percentage and maybe atomic variable
//                K2 *key =item.first;
//                auto it = (job ->interMap)->find(key);
//                if (it == job->interMap->end())
//                {
//                    std::vector <V2 *> vector;
//                    vector.push_back(item.second);
//                    std::pair <K2*, std::vector<V2 *>> pair(item.first, vector);
//                    job->interMap->insert(pair);
//                }
//                else
//                {
//                    it->second.push_back(item.second);
//                }
//            }
        }

    }
    else
        {
        while (true)
        {
            oldVal = (job->jobAtomic)++;
            if (oldVal >= (job->inVec).size())
            {
                job->_jobState->percentage = 100;
                break;
            }
//            job->_client.map(job->inVec[oldVal].first, job->inVec[oldVal].second, job);
            job->_client.map(job->inVec[oldVal].first, job->inVec[oldVal].second, tc);
            job->_jobState->percentage = (float)(oldVal / job->inVec.size()) * 100;
        }
//        job->finishedThreads.push_back(job->runningThreadId);
        job->finishedThreads++;
    }

    // Sort??
//    std::sort(*(job->tc)[job->runningThreadId]->interVec->begin(),
//            *(job->tc)[job->runningThreadId]->interVec->end(), key2Comp);


    //  Shuffle
    job->_jobState->stage = SHUFFLE_STAGE;
    job->_jobState->percentage = 0.0;

    delete (std::vector<IntermediatePair>*)tc->c;

    job->_barrier->barrier();

    // Reduce
    job->_jobState->stage = SHUFFLE_STAGE;
    job->_jobState->percentage = 0.0;

    return 0;
}

JobHandle startMapReduceJob(const MapReduceClient& client, const InputVec& inputVec, OutputVec& outputVec, int multiThreadLevel)
{
    printf("start Map reduce Job\n");
    std::vector<pthread_t*> threads(multiThreadLevel);

    ThreadContext tcs[multiThreadLevel];

    auto *jobContext = new JobContext(client, inputVec, outputVec, new IntermediateMap(),
                                      new JobState({UNDEFINED_STAGE, 0}),
                                      new Barrier(multiThreadLevel), multiThreadLevel);

//    jobContext->shuffleThread = &threadsArr[0];
    for (int i = 0; i < multiThreadLevel - 1; ++i)
    {
        jobContext->runningThreadId = i;
        pthread_create(threads[i], NULL, startRoutine, jobContext);
    }


//    std::vector<V2 *> v2Vec;
//    for (auto item: interMap)
//    {
//        v2Vec.push_back(item.second);
//    }

//    client.reduce(nullptr, v2Vec, nullptr);

    return jobContext;
}

void waitForJob(JobHandle job);
void getJobState(JobHandle job, JobState* state);
void closeJobHandle(JobHandle job);
