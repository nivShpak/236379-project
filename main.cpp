#include <iostream>
#include <fstream>
#include <bitset>
#include <chrono>
#include <unordered_map>
#include <pthread.h>
#include <cstdlib>

#include "vectors.h"
#include "settings.h"

using namespace std;
using namespace std::chrono;

struct max_vector {
    char s_vector[VECTORS_LENGTH + 1];
    int ball_size;
    int mask;
    unordered_map<string, int> *vectors_sizes;
};

int longestRun(string s) {
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

void printAndExportHistogram(int max, unordered_map<string, int> *vectors_sizes, int isPrint, int isExport) {
    // create the histogram - it's not a full one if we check only the at most 2-length runs
    if (!isPrint && !isExport)
        return;

    int i = 0;
    int hist[max + 1];
    ofstream histFile;
    if (isExport) {
        histFile.open(HISTOGRAM_FILE_NAME);
        if (TWO_MAX_RUN_LENGTH)
            histFile << "ball_size,num_vectors(2_max_run_length_only)" << endl;
        else
            histFile << "ball_size,num_vectors" << endl;
    }
    if (isPrint) {
        if (TWO_MAX_RUN_LENGTH)
            cout << "ball_size,num_vectors(2_max_run_length_only)" << endl;
        else
            cout << "ball_size,num_vectors" << endl;
    }

    for (i = 0; i <= max; i++) {
        hist[i] = 0;
    }
    for (auto iter = vectors_sizes->begin(); iter != vectors_sizes->end(); ++iter) {
        auto cur = iter->second;
        // we checked only for vectors that started with '0', so need to duplicate the numbers
        hist[cur]+=2;
    }

    for (i = 0; i <= max; i++) {
        if (hist[i] > 0) {
            if (isPrint)
                cout << i << "," << hist[i] << endl;
            if (isExport)
                histFile << i << "," << hist[i] << endl;
        }
    }
    if (isExport) {
        histFile.close();
        cout << "histogram file created at: " << realpath(HISTOGRAM_FILE_NAME, nullptr);
    }
}

void *splitCheck(void *max_vector_p) {
    auto start = steady_clock::now();
    auto middle = steady_clock::now();
    int total_vectors = 1<<VECTORS_LENGTH;
    int vectors_calculated = 0;
    int tmp_size = 0;
    struct max_vector *max_vector = ((struct max_vector *)max_vector_p);
    unordered_map<string, int> *vectors_sizes = max_vector->vectors_sizes;
    for (int i = max_vector->mask; i < total_vectors; i += NUM_THREADS) {
        string s = bitset<VECTORS_LENGTH>(i).to_string();

        if (s[0] == '1') {
            // it's enough to check ony vectors starting with '0'
            break;
        }
        if (TWO_MAX_RUN_LENGTH && hasRunLongerThan(s, 2)) {
            continue;
        }

        // check if we calculated the reverse
        string s_reverse = s;
        reverse(s_reverse.begin(), s_reverse.end());
        // avoiding the reverse will complicate the calculation of the histogram, so we will do double work in that case
        if ((!(EXPORT_HISTOGRAM || PRINT_HISTOGRAM)) &&
            (vectors_sizes->find(s_reverse) != vectors_sizes->end())) {
            continue;
        }

        vectors_calculated++;
        if (duration_cast<seconds>(steady_clock::now() - middle).count() > 5) {
            if (VERBOSITY >= 2)
                cout << "mask " << max_vector->mask << ": checked " << i/NUM_THREADS << " (calculated " << vectors_calculated << ") of " << total_vectors/NUM_THREADS << endl;
            middle = steady_clock::now();
        }
        vectors v(s);
        tmp_size = v.ballSize();

        vectors_sizes->insert({s, tmp_size});

        if (tmp_size > max_vector->ball_size) {
            max_vector->ball_size = tmp_size;
            strncpy(max_vector->s_vector, v.get_vector().c_str(), VECTORS_LENGTH + 1);
            if (VERBOSITY >= 2)
                cout << max_vector->mask << ": " << max_vector->s_vector << "    " << tmp_size << endl;
            middle = steady_clock::now();
        }
    }

    if (VERBOSITY >= 1) {
        cout << "mask " << max_vector->mask << " calculated " << vectors_calculated << " vectors in "
             << duration_cast<seconds>(steady_clock::now() - start).count() << "s"
             << " max_vector is: " << max_vector->s_vector << " ball size: " << max_vector->ball_size << endl;
    }

    return nullptr;
}

int main() {
    struct max_vector max_vector = {
            .ball_size = 0,
            .mask = 0
    };
    strcpy(max_vector.s_vector, "X");
    // initiate the arguments for the threads
    struct max_vector thread_data[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        auto *vectors_sizes = new unordered_map<string, int>;
        strcpy(thread_data[i].s_vector, "X");
        thread_data[i].ball_size = 0;
        thread_data[i].mask = i;
        thread_data[i].vectors_sizes = vectors_sizes;
    }
    pthread_t ptid[NUM_THREADS];

    // run the balls calculation in parallel
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&ptid[i], nullptr, &splitCheck, (void*)&thread_data[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(ptid[i], nullptr);
    }

    // get the max vector of all the calculations
    for(int i = 0; i < NUM_THREADS; i++) {
        if (thread_data[i].ball_size > max_vector.ball_size) {
            max_vector.ball_size = thread_data[i].ball_size;
            strncpy(max_vector.s_vector, thread_data[i].s_vector, VECTORS_LENGTH + 1);
        }
    }

    cout << endl << "for n=" << VECTORS_LENGTH << " max vector is: " << max_vector.s_vector << " with an indel-2 ball of size: " << max_vector.ball_size << endl << endl;

    if (EXPORT_HISTOGRAM || PRINT_HISTOGRAM) {
        unordered_map<string, int> all_vectors_sizes;
        for (int i=0; i<NUM_THREADS; i++) {
            all_vectors_sizes.merge(*thread_data[i].vectors_sizes);
        }
        printAndExportHistogram(max_vector.ball_size, &all_vectors_sizes, PRINT_HISTOGRAM, EXPORT_HISTOGRAM);
    }

    return 0;
}
