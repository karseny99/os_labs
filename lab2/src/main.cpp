#include <iostream>
#include <pthread.h>
#include <string>
#include <cstdlib>
#include <random>
#include <chrono>

using namespace std;

#define FIRST 1
#define SECOND 2

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
pthread_mutex_t mutex;
int win_counter_1 = 0;

struct pdata {
    int k;
    int cur_round;
    int sum1;
    int sum2;
    int repeat;
};
// int k, int cur_round, int sum1, int sum2
void* who_wins(void* data) {
    pdata* p_data = (pdata*) data; 
    int cur_round = p_data->cur_round;
    int sum1 = p_data->sum1;
    int sum2 = p_data->sum2;
    int k = p_data->k;
    int repeat = p_data->repeat;


    for(int r = 0; r < repeat; ++r) {
        for(int i = cur_round; i <= k; ++i) {
            sum1 += (rng() % 6 + 1);
            sum1 += (rng() % 6 + 1);

            sum2 += (rng() % 6 + 1);
            sum2 += (rng() % 6 + 1);
        }

        if(sum1 >= sum2) {
            pthread_mutex_lock(&mutex);
            ++win_counter_1;
            pthread_mutex_unlock(&mutex);
        }
        sum1 = 0;
        sum2 = 0;
    }


    pthread_exit(0);
}


int main(int argc, char** argv) {
    int k, cur_round, sum1, sum2, exp_num;
    cin >> k >> cur_round >> sum1 >> sum2 >> exp_num;

    if(argc != 2) {
        return -1;
    }
    
    int threadCount = stoi(argv[1]);
    if(threadCount > exp_num) {
        threadCount = exp_num;
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_t tid[threadCount];
    pdata pdata[threadCount];

    int s = 0;
    for(int i = 0; i < threadCount; ++i) {
        if(exp_num > threadCount) {
            pdata[i].repeat = (exp_num + threadCount - 1) / threadCount;
            s += pdata[i].repeat;
        } else {
            pdata[i].repeat = 1;
        }
        pdata[i].cur_round = cur_round;
        pdata[i].k = k;
        pdata[i].sum1 = sum1;
        pdata[i].sum2 = sum2;
    }

    if(exp_num > threadCount) {
        for(int i = 0; i < s - exp_num; ++i) {
            --pdata[i].repeat;
        }
    }

    for(int i = 0; i < threadCount; ++i) {
        pthread_create(&tid[i], NULL, who_wins, &pdata[i]);
    }

    for(int i = 0; i < threadCount; ++i) {
        pthread_join(tid[i], NULL);
    }

    cout.precision(16);
    cout << (double) win_counter_1 / (double) (exp_num) << '\n';
    cout << (double) (exp_num - win_counter_1) / (double) (exp_num) << '\n';

}