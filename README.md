# HNSW-B+tree Filter

An adaptive filtering mechanism that enhances HNSWLib's approximate nearest neighbor (ANN) search capabilities by incorporating a B+ tree for efficient pre-filtering based on attribute ranges. The system dynamically switches between brute-force and HNSW-based ANN search depending on the size of the filtered dataset, optimizing query performance across different scenarios.

## Key Features

- Custom B+ tree implementation optimized for range queries
- Adaptive search strategy selection based on filtered dataset size
- Seamless integration with HNSWLib
- Support for high-dimensional vector data
- Efficient handling of duplicate keys through keeping a vector of data points in each node (that have the same key)
- Comprehensive performance metrics tracking

## Technical Architecture

The system consists of three main components:

1. **B+ Tree Index**
   - Handles range-based filtering
   - Maintains sorted keys for efficient range queries
   - Supports duplicate keys
   - Provides O(log n) search complexity

2. **HNSW Index**
   - Manages high-dimensional vector search
   - Provides approximate nearest neighbor search capabilities
   - Integrated with the filtering mechanism

3. **Adaptive Query Processor**
   - Dynamically selects between brute-force and HNSW search
   - Uses a threshold-based approach (30% of total data points)
   - Optimizes query performance based on filtered set size

## Prerequisites

- C++ compiler with C++11 support

## Installation

1. Clone the repository:
```bash
git clone https://github.com/IlyasIsHere/hnsw-bptree-filter.git
cd hnsw-bptree-filter
```

2. Compile the project:
```bash
g++ -o program main.cpp -Ihnswlib -std=c++11 -O3
```

3. Run the program:
```bash
./program.exe
```

## Testing

The project includes comprehensive unit tests for the B+ Tree implementation. To run the tests:

```bash
g++ ./BPlusTreeTest.cpp -o test
./test
```

## Performance

The system tracks several performance metrics:
- Queries Per Second (QPS)
- Average query runtime
- Total runtime (excluding index building phase)
- Recall rate

### Implementation Details

#### Key Operations Complexity
- Insert: O(log n)
- Search: O(log n)
- Range Query: O(log(n) + m), where m is the number of elements in range

#### Query Processing Pipeline
1. Initial filtering using B+ tree range search
2. Dynamic strategy selection based on filtered ratio
3. Either HNSW with post-filtering or brute-force search on filtered subset

## Challenges and Solutions

1. **Duplicate Key Management**: Implemented Alternative 3 approach using a recordIDs vector in each node.

2. **Parent Tracking**: Added parent pointers in B+ tree nodes to support recursive bottom-up splitting during insertion.

3. **Floating-point Comparison**: Implemented `areAlmostEqual()` function to handle floating-point key comparisons reliably (difference threshold: 0.000001).

4. **HNSW Recall Enhancement**: Developed a two-phase filtering mechanism:
   - Oversampling during HNSW search (k * 10 neighbors)
   - Post-processing to filter candidates based on range constraints
   - Fallback to brute force when necessary

## Contributing

Feel free to open issues or submit pull requests for any improvements or bug fixes.

## Author

Ilyas Hakkou

## Acknowledgments

- Supervised by Mr. Anas Ait Omar and Dr. Karima Echihabi