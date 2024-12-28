#include <cassert>
#include <iostream>
#include "BPlusTree.h"

void testInsertAndTraversal() {
    BPlusTree tree(3); // Increased order for more data
    
    // Test empty tree
    assert(tree.traverseLeaves().empty());
    
    // Insert many values in random order
    tree.insert(5.0f, 1);
    tree.insert(3.0f, 2);
    tree.insert(7.0f, 3);
    tree.insert(1.0f, 4);
    tree.insert(9.0f, 5);
    tree.insert(2.0f, 6);
    tree.insert(4.0f, 7);
    tree.insert(6.0f, 8);
    tree.insert(8.0f, 9);
    tree.insert(10.0f, 10);
    tree.insert(11.0f, 11);
    tree.insert(12.0f, 12);
    tree.insert(13.0f, 13);
    tree.insert(14.0f, 14);
    tree.insert(15.0f, 15);

    // Check if traverseLeaves returns sorted keys
    vector<float> keys = tree.traverseLeaves();
    for (size_t i = 1; i < keys.size(); i++) {
        assert(keys[i-1] <= keys[i]);
    }
    
    cout << "Insert and traversal test passed!" << endl;
}

void testRangeSearch() {
    BPlusTree tree(3);
    
    // Insert many values with duplicates
    for (int i = 1; i <= 5; i++) {
        tree.insert(1.0f, i);     // 5 records with key 1.0
        tree.insert(2.0f, i+5);   // 5 records with key 2.0
        tree.insert(3.0f, i+10);  // 5 records with key 3.0
        tree.insert(4.0f, i+15);  // 5 records with key 4.0
    }
    
    tree.insert(2.5f, 21);
    tree.insert(3.5f, 22);
    tree.insert(1.5f, 23);
    tree.insert(3.2f, 24);
    tree.insert(2.7f, 25);

    // Test range search with many results
    vector<int> result = tree.rangeSearch(2.0f, 3.0f);
    assert(result.size() == 12);  // Should include 5 records for 2.0, 2.5, 2.7, and 5 records for 3.0
    
    // Test range with fewer results
    result = tree.rangeSearch(3.2f, 3.5f);
    assert(result.size() == 2);  // Should include records for 3.2 and 3.5
    
    // Test empty range
    result = tree.rangeSearch(5.0f, 6.0f);
    assert(result.empty());
    
    cout << "Range search test passed!" << endl;
}

void testDuplicateKeys() {
    BPlusTree tree(3);
    
    // Insert many records with the same key
    for (int i = 1; i <= 10; i++) {
        tree.insert(1.0f, i);
        tree.insert(2.0f, i+10);
    }
    
    // Test first key
    vector<int> records = tree.search(1.0f);
    assert(records.size() == 10);
    for (int i = 0; i < 10; i++) {
        assert(records[i] == i+1);
    }
    
    // Test second key
    records = tree.search(2.0f);
    assert(records.size() == 10);
    for (int i = 0; i < 10; i++) {
        assert(records[i] == i+11);
    }
    
    cout << "Duplicate keys test passed!" << endl;
}

int main() {
    cout << "Running B+ Tree tests..." << endl;
    
    testInsertAndTraversal();
    testRangeSearch();
    testDuplicateKeys();
    
    cout << "All tests passed!" << endl;
    return 0;
}