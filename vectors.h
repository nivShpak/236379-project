//
// Created by nivh2 on 2/16/2021.
//

#ifndef UNTITLED2_VECTORS_H
#define UNTITLED2_VECTORS_H

#include <unordered_set>

#include <cstdint>
#include <cmath>
#include <bitset>
#include <string>

using namespace std;


class vectors {
    string s_vector;
    uint8_t size;
    void calcInsertions(unordered_set<string>& hash, string vec);
public:
    explicit vectors(const string &vec): s_vector(vec), size(vec.size()){}
    int ballSize();
    string get_vector()const {
        return this->s_vector;
    }
};


#endif //UNTITLED2_VECTORS_H
