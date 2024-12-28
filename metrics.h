#ifndef METRICS_H
#define METRICS_H

#include <chrono>
#include <vector>
#include <iostream>

struct QueryMetrics {
    double qps;
    double totalRuntime;
    double avgRuntime; 
    double recall;
    int totalQueries;
};

class Timer {
    std::chrono::high_resolution_clock::time_point start;
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0;
    }
};

QueryMetrics computeMetrics(const std::vector<double>& queryTimes, int correctResults, 
                          int totalExpectedResults, double totalRecall = 0.0) {
    QueryMetrics metrics;
    
    // Calculate total runtime
    double totalTime = 0;
    for (double time : queryTimes) {
        totalTime += time;
    }
    metrics.totalRuntime = totalTime;
    metrics.avgRuntime = totalTime / queryTimes.size();
    
    // Calculate QPS
    metrics.qps = queryTimes.size() / totalTime;
    
    // Calculate recall
    metrics.recall = totalRecall / queryTimes.size();  // Average recall across all queries
    
    metrics.totalQueries = queryTimes.size();
    
    return metrics;
}

void displayMetrics(const QueryMetrics& metrics) {
    std::cout << "\n=== Performance Metrics ===\n";
    std::cout << "Queries Per Second (QPS): " << metrics.qps << "\n";
    std::cout << "Total Runtime: " << metrics.totalRuntime << " s\n";
    std::cout << "Average Runtime: " << metrics.avgRuntime << " s\n";
    std::cout << "Recall: " << metrics.recall * 100 << "%\n";
    std::cout << "Total Queries: " << metrics.totalQueries << "\n";
    std::cout << "========================\n\n";
}

#endif 