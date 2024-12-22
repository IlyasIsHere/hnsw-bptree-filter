#include <iostream>
#include "io.h"
#include "BPlusTree.h"

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

int main() {
    string file_path = "dummy-data.bin";
    int num_dimensions = 102;
    vector<vector<float>> data;

    ReadBin(file_path, num_dimensions, data);

    preprocessData(data);

    BPlusTree *bptree = new BPlusTree(2);

    insertVectors(bptree, data, data.size());

    // Perform range query
    float lower_bound = 0.0160491f;
    float upper_bound = 0.0320581f;
    // performRangeQuery(bptree, data, lower_bound, upper_bound);

    // bptree->display();

    vector<float> keys = bptree->traverseLeaves();
    bool isSorted = true;

    for (int i=1; i<keys.size(); i++) {
        if (keys[i-1] > keys[i]) {
            isSorted = false;
            break;
        }
    } 
    cout << "Keys are " << (isSorted ? "sorted" : "not sorted") << endl;

    return 0;
}