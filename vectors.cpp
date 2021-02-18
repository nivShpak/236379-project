//
// Created by nivh2 on 2/16/2021.
//
#include <unordered_map>
#include <vector>
#include <iostream>

#include "vectors.h"
#include "settings.h"

// declarations and global variables
unordered_map<uint64_t, unordered_set<uint64_t>> oneBalls;
void addToAllOneIndel(string s);

int vectors::twoBallSize() {
    unordered_set<string> hash1;
    unordered_set<string> hash2;
    for (int i=0 ; i<this->size-1 ; ++i){
        for (int j = i+1 ; j<this->size ; ++j){
            string tmp_s = this->s_vector;
            tmp_s.erase(j,1);
            tmp_s.erase(i,1);
            if(hash1.find(tmp_s) == hash1.end()) {
                hash1.insert(tmp_s);
                this->calcTwoInsertions(hash2, tmp_s);
            }
        }
    }
    return hash2.size();
}

void vectors::calcTwoInsertions(unordered_set<string>& hash, string vec){
    string tmp;
    for(int i=0 ; i<this->size-1 ; ++i){
        for (int j = i+1; j < this->size; ++j) {
            tmp = vec;
            tmp.insert(i,"0");
            tmp.insert(j,"0");
            hash.insert(tmp);

            tmp = vec;
            tmp.insert(i,"1");
            tmp.insert(j,"0");
            hash.insert(tmp);

            tmp = vec;
            tmp.insert(i,"0");
            tmp.insert(j,"1");
            hash.insert(tmp);

            tmp = vec;
            tmp.insert(i,"1");
            tmp.insert(j,"1");
            hash.insert(tmp);
        }
    }
}

// add to the big oneBalls map, all reachable vectors from this
void addToAllOneIndel(string s) {
    // check if this was calculated already. If so, nothing to do here
    uint64_t number = strtoull(s.c_str(), nullptr, 2);
    if (oneBalls.find(number) != oneBalls.end()) {
        return;
    }

    unordered_set<uint64_t> radiusOne;
    uint64_t  tmp_number;
    for (int i=0 ; i < VECTORS_LENGTH ; ++i) {
        string tmp_a = s;
        tmp_a.erase(i, 1);
        for (int j = 0; j < VECTORS_LENGTH; ++j) {
            string tmp_b = tmp_a;
            tmp_b.insert(j, "0");
            tmp_number = stoull(tmp_b, nullptr, 2);
            radiusOne.insert(tmp_number);

            tmp_b = tmp_a;
            tmp_b.insert(j, "1");
            tmp_number = stoull(tmp_b, nullptr, 2);
            radiusOne.insert(tmp_number);
        }
    }
    oneBalls.insert({number, radiusOne});
}

int vectors::oneBallSize() {
    uint64_t number = strtoull(this->s_vector.c_str(), nullptr, 2);
    auto radius = oneBalls.find(number);
    // check if it was calculated already
    if (radius != oneBalls.end()) {
        return radius->second.size();
    } else {
        addToAllOneIndel(this->s_vector);
        return this->oneBallSize();
    }
}

int vectors::twoBallSizeFromHash() {
    unordered_set<uint64_t> hash1;
    for (int i=0 ; i < this->size ; ++i) {
        string tmp_a = this->s_vector;
        tmp_a.erase(i, 1);
        // add to oneBalls it's 1-indel vectors
        for (int j = 0; j < this->size; ++j) {
            string tmp_b = tmp_a;
            tmp_b = tmp_b.insert(j, "0");
            addToAllOneIndel(tmp_b);
            auto vectors = oneBalls.find(strtoull(tmp_b.c_str(), nullptr, 2));
            for (auto it = vectors->second.begin(); it != vectors->second.end(); ++it) {
                hash1.insert(*it);
            }

            tmp_b = tmp_a;
            tmp_b = tmp_b.insert(j, "1");
            addToAllOneIndel(tmp_b);
            vectors = oneBalls.find(strtoull(tmp_b.c_str(), nullptr, 2));
            for (auto it = vectors->second.begin(); it != vectors->second.end(); ++it) {
                hash1.insert(*it);
            }
        }
    }
    return hash1.size();
}

void showCalculatedVectors() {
    int i =0;
    for (auto it = oneBalls.begin(); it != oneBalls.end(); ++it) {
        i++;
        cout << i << " - source: " << it->first << endl;
        for (auto jt = it->second.begin(); jt != it->second.end(); ++jt) {
            cout << "\t" << *jt << endl;
        }
    }
}