#include <iostream>
#include <bitset>
#include "vectors.h"
#include <chrono>
#include <unordered_map>
#include <pthread.h>

using namespace std;
using namespace std::chrono;

#define VECTORS_LENGTH 26
#define NUM_THREADS    10

struct max_vector {
    string s_vector;
    int ball_size;
    int mask;
};

int longestRun(string s){
    int max_run = 0;
    int current_run=0;
    int i = 0;
    char curr_char = s[0];
    char next_char;
    while(i < s.size()) {
        next_char = s[i];
        if (next_char == curr_char)
            current_run++;
        else
            current_run = 1;
        if (current_run > max_run)
            max_run = current_run;
        i++;
        curr_char = next_char;
    }
    return max_run;
}

bool hasRunLongerThan(string s, int x) {
    int current_run=0;
    int i = 0;
    char curr_char = s[0];
    char next_char;
    while(i < s.size()) {
        next_char = s[i];
        if (next_char == curr_char)
            current_run++;
        else
            current_run = 1;
        if (current_run > x)
            return true;
        i++;
        curr_char = next_char;
    }
    return false;
}

void printHistogram(int max, unordered_map<string, int> vectors_sizes) {
    // create the histogram - it's not a full one if we check only the at most 2-length runs
    int i = 0;
    int hist[max + 1];
    for (i = 0; i <= max; i++) {
        hist[i] = 0;
    }
    for(auto iter = vectors_sizes.begin(); iter != vectors_sizes.end(); ++iter) {
        auto cur = iter->second;
        hist[cur]++;
        //cout << "first is: " << iter->first << " second is: " << iter->second << endl;
    }
    cout << "sizes histogram:" << endl;
    for (i = 0; i <= max; i++) {
        if (hist[i] > 0)
            cout << i << "    " << hist[i] << endl;
    }

}

void *splitCheck(void *max_vector_p) {
    auto start = steady_clock::now();
    auto middle = steady_clock::now();
    unordered_map<string, int> vectors_sizes;
    int total_vectors = 1<<VECTORS_LENGTH;
    uint64_t total_time_calculating_balls = 0;
    int vectors_calculated = 0;
    int tmp_size = 0;
    auto max_vector = ((struct max_vector *)max_vector_p);;

    for (int i = max_vector->mask; i < total_vectors; i += NUM_THREADS) {
        string s = bitset<VECTORS_LENGTH>(i).to_string();

        if (s[0] == '1') {
            // it's enough to check ony vectors starting with '0'
            break;
        }
        if (hasRunLongerThan(s, 2)) {
            continue;
        }

        // check if we calculated the not or the reverse
        string s_reverse = s;
        string s_not = s;
        for (int index = 0; index < s_not.size(); index++) {
            s_not[index] == '1' ? s_not[index] = '0' : s_not[index] = '1';
        }
        reverse(s_reverse.begin(), s_reverse.end());
        // the vector with the most vectors inside it's 2-indel ball, it's longest gun will be of 2 at most
        if ((vectors_sizes.find(s_reverse) != vectors_sizes.end()) ||
            (vectors_sizes.find(s_not) != vectors_sizes.end())) {
            continue;
        }

        vectors_calculated++;
        if (duration_cast<seconds>(steady_clock::now() - middle).count() > 5) {
            cout << "mask " << max_vector->mask << ": checked " << i/8 << " (calculated " << vectors_calculated << ") of " << total_vectors/8 << endl;
            middle = steady_clock::now();
        }
        auto ballStart = steady_clock::now();
        vectors v(s);
        tmp_size = v.ballSize();

        vectors_sizes.insert({s, tmp_size});

        if (tmp_size > max_vector->ball_size) {
            max_vector->ball_size = tmp_size;
            max_vector->s_vector = v.get_vector();
            // cout << max_vector.mask << ": " << max_vector.s_vector << "    " << tmp_size << endl;
            middle = steady_clock::now();
        }
        total_time_calculating_balls += duration_cast<nanoseconds>(steady_clock::now() - ballStart).count();
    }
    cout << "mask " << max_vector->mask << " calculated " << vectors_calculated << " vectors in " << total_time_calculating_balls / 1000000000 << "s" << endl;

    return nullptr;
}

int main() {
    struct max_vector max_vector = {
            .s_vector = "0",
            .ball_size = -1,
            .mask = -1
    };
    struct max_vector tmp_vectors[8]; // = (struct max_vector *)malloc(sizeof(struct max_vector)*8);
    pthread_t ptid[8];

    for(int i = 0; i < NUM_THREADS; i++) {
        tmp_vectors[i].mask = i;
        pthread_create(&ptid[i], nullptr, &splitCheck, (void*)(tmp_vectors + i));
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(ptid[i], nullptr);
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        if (tmp_vectors[i].ball_size > max_vector.ball_size)
            max_vector = tmp_vectors[i];
    }

    cout << "for n=" << VECTORS_LENGTH << " max vector is: " << max_vector.s_vector << " with an indel-2 ball of size: " << max_vector.ball_size << endl;
    //printHistogram(max, vectors_sizes);

    return 0;
}


