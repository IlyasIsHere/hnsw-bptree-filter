#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include "BPlusTree.h"
#include "hnswlib/hnswlib.h"
#include <queue>

using namespace std;

// Function to preprocess data by removing categorical attribute
void preprocessData(vector<vector<float>>& data) {
    for (auto &vec: data) {
        vec.erase(vec.begin());
    }
}

// Function to insert all vectors into B+ tree
void insertVectors(BPlusTree* tree, const vector<vector<float>>& data, int numVectors) {
    for (int i = 0; i < numVectors; i++) {
        tree->insert(data[i][0], i);
    }
}

// Function to print vector data
void printVector(const vector<float>& vec, int id) {
    cout << "Vector " << id << ": [";
    for (int i = 0; i < min(5, (int)vec.size()); i++) {
        cout << vec[i];
        if (i < min(4, (int)vec.size() - 1)) cout << ", ";
    }
    if (vec.size() > 5) cout << ", ...";
    cout << "]" << endl;
}

// Function to perform and display range query results
void performRangeQuery(BPlusTree* tree, const vector<vector<float>>& data, float lower, float upper) {
    cout << "\nPerforming range query for vectors with keys between " 
         << lower << " and " << upper << ":" << endl;

    auto searchResults = tree->rangeSearch(lower, upper);
    cout << "Found " << searchResults.size() << " results:" << endl;
    
    for (auto &id: searchResults) {
        printVector(data[id], id);
    }
}

// Function to check if vector is sorted
bool checkIfSorted(const vector<float>& keys) {
    for (int i=1; i<keys.size(); i++) {
        if (keys[i-1] > keys[i]) {
            return false;
        }
    }
    return true;
}

float computeDistance(vector<float> a, vector<float> b) {
    float dist = 0;
    assert(a.size() == b.size());
    float diff;
    
    for (int i=0; i<a.size(); i++) {
        diff = a[i] - b[i];
        dist += diff * diff;
    }
    return dist;
}

vector<int> getKNN(int k, vector<vector<float>> &data, vector<int> candidatesIDs, vector<float> &queryVector) {
    // Max heap to store k-nearest neighbors (distance, id)
    priority_queue<pair<float, int>> max_heap;
    vector<int> result;
    result.reserve(k);  // Pre-allocate space for efficiency

    for (auto &id: candidatesIDs) {
        // Skip first dimension (range attribute) when computing distance
        vector<float> vec(data[id].begin() + 1, data[id].end());
        float dist = computeDistance(vec, queryVector);
        
        max_heap.push({dist, id});
        if (max_heap.size() > k) {
            max_heap.pop();
        }

    }

    // Convert max heap to vector (will be in descending order)
    while (!max_heap.empty()) {
        result.push_back(max_heap.top().second);
        max_heap.pop();
    }

    // Reverse to get ascending order
    reverse(result.begin(), result.end());

    return result;
}

#endif // UTILS/H 