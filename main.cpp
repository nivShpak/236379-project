#include <iostream>
#include <string>
#include <bitset>
#include "vectors.h"

using namespace std;

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
    for (unsigned int i=0 ; i<INTMAX_MAX; ++i) {
        string s = bitset<25>(i).to_string();
        vectors v(s);
        if (longestRun(s)<=2) {
            int tmp = v.ballSize();
            if (tmp > max) {
                max = tmp;
                cout << v.get_vector() << "    " << max << "     " << endl;
            }
        }
    }
    return 0;
}




