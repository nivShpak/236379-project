#include <iostream>
#include <fstream>
#include <bitset>
#include <chrono>
#include <unordered_map>
#include <pthread.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iterator>

#include "vectors.h"
#include "settings.h"

using namespace std;
using namespace std::chrono;

extern uint32_t skippedVectors;
extern uint32_t skippedOperations;

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
    int hist[(max / HISTOGRAM_BUCKET_SIZE) + 1];
    ofstream histFile;
    uint64_t totalBallsSum = 0, totalVectors = 0;

    if (isExport) {
        histFile.open(HISTOGRAM_FILE_NAME);
        if (TWO_MAX_RUN_LENGTH)
            histFile << "ball_size_bucket,num_vectors(2_max_run_length_only)" << endl;
        else
            histFile << "ball_size_bucket,num_vectors" << endl;
    }
    if (isPrint) {
        if (TWO_MAX_RUN_LENGTH)
            cout << "ball_size_bucket,num_vectors(2_max_run_length_only)" << endl;
        else
            cout << "ball_size_bucket,num_vectors" << endl;
    }

    for (i = 0; i <= max/HISTOGRAM_BUCKET_SIZE; i++) {
        hist[i] = 0;
    }
    for (auto iter = vectors_sizes->begin(); iter != vectors_sizes->end(); ++iter) {
        auto cur = iter->second;
        // we checked only for vectors that started with '0', so need to duplicate the numbers
        hist[cur/HISTOGRAM_BUCKET_SIZE]+=2;
        totalBallsSum += cur;
        totalVectors++;
    }

    for (i = 0; i <= max/HISTOGRAM_BUCKET_SIZE; i++) {
        if (hist[i] > 0) {
            if (isPrint)
                cout << i*HISTOGRAM_BUCKET_SIZE << "," << hist[i] << endl;
            if (isExport)
                histFile << i*HISTOGRAM_BUCKET_SIZE << "," << hist[i] << endl;
            // calculate the sum to print the avg ball size
        }
    }
    if (isExport) {
        histFile.close();
        cout << "histogram file created at: " << realpath(HISTOGRAM_FILE_NAME, nullptr) << endl;
    }
    cout << "avg ball size is: " << totalBallsSum / totalVectors << " (from " << totalVectors*2 << " vectors)" << endl;
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

        if (VERBOSITY >= 1) {
            if (duration_cast<seconds>(steady_clock::now() - middle).count() > 5)
                cout << "mask " << max_vector->mask << ": checked " << i/NUM_THREADS << " (calculated " << vectors_calculated << ") of " << total_vectors/NUM_THREADS << endl;
            middle = steady_clock::now();
        }
        vectors v(s);
        tmp_size = v.twoBallSize();
        vectors_calculated++;

        vectors_sizes->insert({s, tmp_size});

        if (tmp_size > max_vector->ball_size) {
            max_vector->ball_size = tmp_size;
            strncpy(max_vector->s_vector, v.get_vector().c_str(), VECTORS_LENGTH + 1);
            if (VERBOSITY >= 1) {
                if (VERBOSITY >= 2)
                    cout << max_vector->mask << ": " << max_vector->s_vector << "    " << tmp_size << endl;
                middle = steady_clock::now();
            }
        }
    }

    if (VERBOSITY >= 1) {
        cout << "mask " << max_vector->mask << " calculated " << vectors_calculated << " vectors in "
             << duration_cast<seconds>(steady_clock::now() - start).count() << "s"
             << " max_vector is: " << max_vector->s_vector << " ball size: " << max_vector->ball_size << endl;
    }
    return nullptr;
}

void initiateMaxVector(struct max_vector &v, int mask = -1) {
    strcpy(v.s_vector, "X");
    v.ball_size = 0;
    v.vectors_sizes = new unordered_map<string, int>;
    v.mask = mask;
}

int main() {
    struct max_vector max_vector;
    initiateMaxVector(max_vector);
    // initiate the arguments for the threads
    struct max_vector thread_data[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        initiateMaxVector(thread_data[i], i);
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

    unordered_map<string, int> all_vectors_sizes;
    for (int i=0; i<NUM_THREADS; i++) {
        for (auto it = thread_data[i].vectors_sizes->begin(); it != thread_data[i].vectors_sizes->end(); ++it) {
            all_vectors_sizes.insert({it->first, it->second});
        }
    }
    if (EXPORT_HISTOGRAM || PRINT_HISTOGRAM) {
        printAndExportHistogram(max_vector.ball_size, &all_vectors_sizes, PRINT_HISTOGRAM, EXPORT_HISTOGRAM);
    }

    cout << endl;
    if (VERBOSITY >= 1) {
        cout << "skipped " << skippedVectors << " vectors in the middle, reducing " << skippedOperations << " calcTwoInsertions() operations" << endl;
    }
    // print the max ball size, and all it's vectors. Create the export file as well
    cout << "for n=" << VECTORS_LENGTH << " max indel-2 ball size is " << max_vector.ball_size
         << ". max vectors are:" << endl;
    ofstream outputFile;
    if (OUTPUT_MAX_VECTORS > 0) {
        outputFile.open(MAX_VECTORS_FILE);
        outputFile << "n" << "," << "vector" << "," << "ball_size" << endl;
    }
        for (auto iter = all_vectors_sizes.begin(); iter != all_vectors_sizes.end(); ++iter) {
        if (iter->second == max_vector.ball_size) {
            cout << "\t" << iter->first << endl;
            if (OUTPUT_MAX_VECTORS > 0) {
                outputFile << VECTORS_LENGTH << "," << iter->first << "," << iter->second << endl;
            }
        }
    }
    cout << endl;

    // cleanup
    for (int i = 0; i < NUM_THREADS; i++) {
        delete thread_data[i].vectors_sizes;
    }
    delete max_vector.vectors_sizes;

    return 0;
}
