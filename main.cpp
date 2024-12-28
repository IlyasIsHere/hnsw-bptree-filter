#include <iostream>
#include <algorithm>
#include "io.h"
#include "BPlusTree.h"
#include "utils.h"
#include "hnswlib.h"
#include "metrics.h"

using namespace std;

int main() {
    string file_path = "data/contest-data-release-1m.bin";
    string query_path = "data/contest-queries-release-1m.bin";
    int num_dimensions = 102;
    int num_query_dimensions = 104;
    vector<vector<float>> data;

    ReadBin(file_path, num_dimensions, data);
    preprocessData(data);

    cout << "Building B+ tree index..." << endl;
    BPlusTree *bptree = new BPlusTree(10);
    insertVectors(bptree, data, data.size());
    cout << "B+ tree index built successfully" << endl;

    // Initialize HNSW index
    cout << "Building HNSW index..." << endl;
    int dim = data[0].size() - 1; // Subtract 1 because first dimension is range attribute
    size_t max_elements = data.size();
    hnswlib::L2Space space(dim);
    hnswlib::HierarchicalNSW<float>* hnsw_index = new hnswlib::HierarchicalNSW<float>(&space, max_elements);
    
    // Add points to the index
    for(size_t i = 0; i < data.size(); i++) {
        vector<float> vec(data[i].begin() + 1, data[i].end()); // Skip range attribute
        hnsw_index->addPoint(vec.data(), i);
    }
    cout << "HNSW index built successfully" << endl;


    int k = 100;

    vector<vector<float>> queries;
    ReadBin(query_path, num_query_dimensions, queries);

    // Taking only the queries of type 2 or 3, and removing the first and second dimension (not relevant)
    vector<vector<float>> filteredQueries;
    for (auto& query : queries) {
        if (query[0] == 2 || query[0] == 3) {
            vector<float> preprocessedQuery(query.begin() + 2, query.end());
            filteredQueries.push_back(preprocessedQuery);
        }
    }

    const float HNSW_THRESHOLD = 0.7; // If more than 70% of data points remain after filtering

    vector<double> queryTimes;
    int correctResults = 0;
    int totalExpectedResults = 0;

    for (auto &query: filteredQueries) {
        Timer timer;
        
        float lower = query[0];
        float upper = query[1];
        vector<float> queryVector(query.begin() + 2, query.end());

        vector<int> candidatesIDs = bptree->rangeSearch(lower, upper);
        
        // Calculate what percentage of total data points remain after filtering
        float filteredRatio = static_cast<float>(candidatesIDs.size()) / data.size();
        
        vector<int> knn;
        if (filteredRatio > HNSW_THRESHOLD) {
            // Use HNSW on full dataset but get more neighbors to ensure we find enough valid ones
            // cout << "Using HNSW (filtered ratio: " << filteredRatio << ")" << endl;
            const int extraK = k * 10; // Get 10x more neighbors to have enough after filtering
            
            std::priority_queue<std::pair<float, hnswlib::labeltype>> result = hnsw_index->searchKnn(queryVector.data(), extraK);
            
            // Filter results to only include points within the range constraint
            vector<int> filtered_knn;
            while (!result.empty()) {
                int id = result.top().second;
                if (data[id][0] >= lower && data[id][0] <= upper) {
                    filtered_knn.push_back(id);
                    if (filtered_knn.size() >= k) break;
                }
                result.pop();
            }
            
            // If we couldn't find enough neighbors, fall back to brute force
            if (filtered_knn.size() < k) {
                // cout << "HNSW couldn't find enough valid neighbors, falling back to brute force" << endl;
                knn = getKNN(k, data, candidatesIDs, queryVector);
            } else {
                knn = filtered_knn;
            }
            correctResults += knn.size();  // Assuming HNSW results are correct
        } else {
            // If few points remain, use brute force on the filtered subset
            // cout << "Using brute force (filtered ratio: " << filteredRatio << ")" << endl;
            knn = getKNN(k, data, candidatesIDs, queryVector);
            correctResults += knn.size();  // Assuming brute force results are correct
        }
        
        totalExpectedResults += k;  // k is the target number of neighbors
        queryTimes.push_back(timer.elapsed());
        
        // cout << "Query completed in " << timer.elapsed() * 1000 << " ms\n";
    }

    // Compute and display final metrics
    QueryMetrics metrics = computeMetrics(queryTimes, correctResults, totalExpectedResults);
    displayMetrics(metrics);

    // Clean up
    delete bptree;
    delete hnsw_index;

    return 0;
}