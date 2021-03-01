//
// Created by nivh2 on 2/16/2021.
//
#include <unordered_map>

#include "vectors.h"
#include "settings.h"

uint64_t maxBallSize;
uint64_t skippedVectors;
uint64_t skippedBallsCalculations;
uint64_t executedBallsCalculations;

int getNumRuns(const string& s) {
    int numRuns = 1;
    for (int i = 1; i < s.size(); i++) {
        if (s[i] == s[i-1])
            continue;
        else
            numRuns++;
    }
    return numRuns;
}

static inline int deletionBallUpperBound(const string& s) {
    // get the number of runs
    int runs = getNumRuns(s);
    return (runs * (runs - 1))/2;
}

int64_t vectors::twoBallSize() {
    // optimization to stop the calculation before doing any work.
    if ((!IS_HISTOGRAM) && (deletionBallUpperBound(this->s_vector) * TWO_INSERTIONS_BALL_SIZE) < maxBallSize) {
        skippedVectors++;
        return -1;
    }

    // unordered_set<string> hash2;
    // hash2.max_load_factor(0.25);hash2.reserve(500);
    unordered_set<string> hash1;
    hash1.max_load_factor(0.25);hash1.reserve(500);

    string s = this->s_vector;
    for(int i=0; i<this->size-1; ++i) {
        if(i < this->size-2) {
            if (s[i]==s[i+1] and s[i]==s[i+2]) continue;
        }
        for(int j = i+1 ; j < this->size ; ++j) {
            if (j < this->size-1) {
                if (s[j]==s[j+1]) continue;
            }
            string tmp_s = s;
            tmp_s.erase(j,1);
            tmp_s.erase(i,1);
            if(hash1.find(tmp_s) == hash1.end()) {
                hash1.insert(tmp_s);
                // this->calcTwoInsertions(hash2, tmp_s);
            }
        }
    }
    return this->calcTwoInsertionsWithStop(hash1);
    // return hash2.size();
}

void insert_j(int j, const string& s, unordered_set<string>& hash, char prev_j) {
    string tmp;
    if (prev_j != '0') {
        tmp = s;
        tmp.insert(j, "0");
        hash.insert(tmp);
    }

    if (prev_j != '1') {
        tmp = s;
        tmp.insert(j, "1");
        hash.insert(tmp);
    }
}

void vectors::calcTwoInsertions(unordered_set<string>& hash, string vec){
    executedBallsCalculations++;
    string tmp;
    char prev_i = 'x';
    char prev_j = 'x';
    for(int i=0 ; i < this->size-1 ; ++i) {
        if (prev_i != '0') {
            tmp = vec;
            tmp.insert(i, "0");
            for (int j = i + 1; j < this->size; ++j) {
                insert_j(j, tmp, hash, prev_j);
                prev_j = tmp[j];
            }
        }
        if (prev_i != '1') {
            tmp = vec;
            tmp.insert(i, "1");
            for (int j = i + 1; j < this->size; ++j) {
                insert_j(j, tmp, hash, prev_j);
                prev_j = tmp[j];
            }
        }
        prev_i = vec[i];
    }
}

int64_t vectors::calcTwoInsertionsWithStop(unordered_set<string> &hash1) {
    unordered_set<string> hash2;
    hash2.reserve(2*maxBallSize);
    int hash1Size = hash1.size();
    uint32_t potentialVectors = hash1Size * TWO_INSERTIONS_BALL_SIZE;

    int i = 0;
    for (auto it = hash1.begin(); it != hash1.end(); ++it) {
        if ( (!IS_HISTOGRAM) && potentialVectors < maxBallSize) {
            skippedBallsCalculations += (hash1Size - i);
            return -1;
        }
        i++;
        calcTwoInsertions(hash2, *it);
        potentialVectors = hash2.size() + ((hash1Size - i) * TWO_INSERTIONS_BALL_SIZE);
    }

    int64_t thisBallSize = hash2.size();
    if (thisBallSize > maxBallSize) {
        maxBallSize = thisBallSize;
    }
    return thisBallSize;
}
