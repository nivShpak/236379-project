#include <iostream>
#include <string>
#include <bitset>
#include "vectors.h"
#include <chrono>
#include <unordered_map>

using namespace std;
using namespace std::chrono;

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

int main() {
    int max = 0;
    auto start = steady_clock::now();
    auto middle = steady_clock::now();
    unsigned int i, j=0;
    unordered_map<string, int> vectors_sizes;
    int total_vectors = 1<<25;
    for (i=0 ; i < total_vectors; ++i) {
        string s = bitset<25>(i).to_string();

        if (s[0] == '1') {
            // it's enough to check ony vectors starting with '0'
            break;
        }
        if (duration_cast<seconds>(steady_clock::now() - middle).count() > 3) {
            cout << duration_cast<seconds>(steady_clock::now() - start).count() << ": " << i << " of " << total_vectors << endl;
            middle = steady_clock::now();
        }

        // check if we calculated the not or the reverse
        string s_reverse = s;
        string s_not = s;
        for (int index = 0; index < s_not.size(); index++) {
            s_not[index] == '1' ? s_not[index] = '0' : s_not[index] = '1';
        }
        reverse(s_reverse.begin(), s_reverse.end());
        // the vector with the most vectors inside it's 2-indel ball, it's longest gun will be of 2 at most
        if (longestRun(s) > 2 ||
            (vectors_sizes.find(s_reverse) != vectors_sizes.end()) ||
            (vectors_sizes.find(s_not) != vectors_sizes.end())) {
            continue;
        }

        j++;
        vectors v(s);
        int tmp = v.ballSize();

        vectors_sizes.insert({s, tmp});

        if (tmp > max) {
            max = tmp;
            cout << duration_cast<seconds>(steady_clock::now() - start).count() << ": " << v.get_vector() << "    " << max << "     " << endl;
            middle = steady_clock::now();
        }
    }
    auto end = steady_clock::now();
    cout << "2-indel check of " << j << " vectors out of " << i*2 << " took: " << duration_cast<seconds>(end - start).count() << "s" << endl;

    // create the histogram - it's not a full one if we check only the at most 2-length runs
    int hist[max];
    for (i = 0; i < max; i++) {
        hist[i] = 0;
    }
    for(auto iter = vectors_sizes.begin(); iter != vectors_sizes.end(); ++iter) {
        auto cur = iter->second;
        hist[cur]++;
        //cout << "first is: " << iter->first << " second is: " << iter->second << endl;
    }
    cout << "sizes histogram:" << endl;
    for (i = 0; i < max; i++) {
        if (hist[i] > 0)
            cout << i << "    " << hist[i] << endl;
    }
    return 0;
}


