//
// Created by nivh2 on 2/16/2021.
//

#include "vectors.h"


int vectors::ballSize() {
    unordered_set<string> hash1;
    unordered_set<string> hash2;
    for (int i=0 ; i<this->size-1 ; ++i){
        for (int j = i+1 ; j<this->size ; ++j){
            string tmp_s = this->s_vector;
            tmp_s.erase(j,1);
            tmp_s.erase(i,1);
            if(hash1.find(tmp_s)== hash1.end()) {
                hash1.insert(tmp_s);
                this->calcInsertions(hash2, tmp_s);
            }
        }
    }
    return hash2.size();
}

void vectors::calcInsertions(unordered_set<string>& hash, string vec){
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
