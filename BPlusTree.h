#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <utility>
#include <cmath>
#include <iostream>
#include <string>

using namespace std;

/**
 * @brief Checks if two floating point numbers are approximately equal
 * @param a First number to compare
 * @param b Second number to compare
 * @return true if numbers are within epsilon of each other, false otherwise
 */
bool areAlmostEqual(float a, float b) { 
    const float eps = 0.000001f;
    return fabs(a - b) < eps; 
} 

/**
 * @brief B+ tree implementation for indexing float keys with associated record IDs
 * 
 * This B+ tree maintains sorted float keys in leaf nodes, with each key potentially 
 * mapping to multiple record IDs (alternative 3 to handle duplicates). Internal nodes store keys for tree traversal.
 * Leaf nodes are linked together to facilitate range queries.
 */
class BPlusTree {
private:
    /**
     * @brief Node structure for B+ tree
     * 
     * Can be either an internal node (containing keys and child pointers) or
     * a leaf node (containing keys and record IDs). Leaf nodes are linked together.
     */
    struct Node {
        bool isLeaf;
        vector<float> keys;
        vector<vector<int>> recordIDs; // i-th element is a vector of the IDs of the vector data points having key == keys[i] (to handle duplicates) (only used for leaf nodes)
        vector<Node *> children; // This would have a length of keys.size() + 1
        Node *parent;
        Node *next; // Used only for leaf nodes (for range queries)

        /**
         * @brief Constructs a new Node
         * @param isLeaf Whether this is a leaf node
         */
        Node(bool isLeaf = false) {
            this->isLeaf = isLeaf;
            parent = nullptr;
            next = nullptr;
        }
    };

    Node *root;
    int d; // The order of the B+Tree (the max number of keys in a node is 2*d)

    /**
     * @brief Finds the leaf node where a key should be located
     * @param key The key to search for
     * @return Pointer to the leaf node where key belongs
     */
    Node *getLeaf(float key);

    /**
     * @brief Splits an overfull leaf node
     * @param leafNode The leaf node to split
     */
    void splitLeaf(Node *leafNode);

    /**
     * @brief Inserts a key and child pointer into an internal node
     * @param currNode The internal node to insert into
     * @param key The key to insert
     * @param newChild Pointer to the new child node
     * @return Pair containing the inserted key and child pointer
     */
    pair<float, Node*> insertInternal(Node *currNode, float key, Node *newChild);

    /**
     * @brief Splits an overfull internal node
     * @param internalNode The internal node to split
     */
    void splitInternal(Node *internalNode);

    /**
     * @brief Recursively deletes a node and all its children
     * @param node The root of the subtree to delete
     */
    void deleteTree(Node* node) {
        if (node != nullptr) {
            if (!node->isLeaf) {
                for (Node* child : node->children) {
                    deleteTree(child);
                }
            }
            delete node;
        }
    }

public:
    /**
     * @brief Constructs a new B+ tree
     * @param order The order d of the tree (max 2d keys per node)
     */
    BPlusTree(int order) : d(order), root(nullptr) {}

    /**
     * @brief Destructor that cleans up all nodes
     */
    ~BPlusTree() {
        deleteTree(root);
    }

    /**
     * @brief Inserts a new key-recordID pair into the tree
     * @param key The key to insert
     * @param recordID The record ID associated with the key
     */
    void insert(float key, int recordID);

    /**
     * @brief Searches for all record IDs associated with a key
     * @param key The key to search for
     * @return Vector of record IDs matching the key
     */
    vector<int> search(float key);

    /**
     * @brief Finds all record IDs with keys in the given range
     * @param lower Lower bound of the key range (inclusive)
     * @param upper Upper bound of the key range (inclusive)
     * @return Vector of record IDs within the range
     */
    vector<int> rangeSearch(float lower, float upper);

    /**
     * @brief Displays the tree structure
     */
    void display() { 
        cout << "\nB+ Tree Structure:\n" << endl;
        displayTree(root, 0); 
    }

    /**
     * @brief Helper function to recursively display the tree
     * @param node Current node to display
     * @param level Current level in the tree (for indentation)
     */
    void displayTree(Node* node, int level);

    /**
     * @brief Returns all keys in the tree in sorted order
     * @return Vector of keys from traversing leaf nodes left to right
     */
    vector<float> traverseLeaves();
};

BPlusTree::Node *BPlusTree::getLeaf(float key) {
    if (this->root == nullptr) return nullptr;

    Node *currPtr = this->root;

    while (!currPtr->isLeaf) {
        int i = 0;
        // Placing i in the right place
        while (i < currPtr->keys.size() and key >= currPtr->keys[i]) {
            i++;
        }
        currPtr = currPtr->children[i];
    }
    // currPtr is pointing to a leaf now
    return currPtr;
}

vector<int> BPlusTree::search(float key) {
    if (this->root == nullptr) return {};
    Node *leafPtr = getLeaf(key);

    for (int i = 0; i < leafPtr->keys.size(); i++) {
        if (areAlmostEqual(key, leafPtr->keys[i])) {
            return leafPtr->recordIDs[i];
        }
    }
    return {}; // if not found
}

vector<int> BPlusTree::rangeSearch(float lower, float upper) {
    if (this->root == nullptr) return {};
    Node *leafPtr = getLeaf(lower);

    vector<int> result;
    while (leafPtr != nullptr) {
        for (int i = 0; i < leafPtr->keys.size(); i++) {
            auto key = leafPtr->keys[i];
            auto data = leafPtr->recordIDs[i];
            if (lower <= key and key <= upper) {
                result.insert(result.end(), data.begin(), data.end()); // Adding the current recordIDs to the result
            }
            if (key > upper) return result;
        }
        leafPtr = leafPtr->next;
    }
    return result;
}

void BPlusTree::insert(float key, int recordID) {
    // If the tree is empty
    if (root == nullptr) {
        root = new Node(true);
        root->keys.push_back(key);
        root->recordIDs.push_back({recordID});
    } else {
        Node *leafNode = getLeaf(key);

        // Finding where to insert (so that keys remain sorted)
        auto it = lower_bound(leafNode->keys.begin(), leafNode->keys.end(), key);
        int pos = it - leafNode->keys.begin();

        if (it != leafNode->keys.end() && areAlmostEqual(*it, key)) {
            // If the key already exists
            leafNode->recordIDs[pos].push_back(recordID);
        } else {
            leafNode->keys.insert(it, key);
            leafNode->recordIDs.insert(leafNode->recordIDs.begin() + pos, {recordID});
        }

        // Check if the leaf node is overfull
        if (leafNode->keys.size() > 2 * d) {
            splitLeaf(leafNode);
        }
    }
}

void BPlusTree::splitLeaf(BPlusTree::Node *leafNode) {
    Node *newLeaf = new Node(true);

    int splitIndex = d; // Middle index
    float newKey = leafNode->keys[splitIndex];

    // Move second half of keys and recordIDs to the new leaf (by convention, the median key goes to the new leaf)
    newLeaf->keys.assign(leafNode->keys.begin() + splitIndex, leafNode->keys.end());
    newLeaf->recordIDs.assign(leafNode->recordIDs.begin() + splitIndex, leafNode->recordIDs.end());

    leafNode->keys.resize(splitIndex);
    leafNode->recordIDs.resize(splitIndex);

    // Update the linked list
    newLeaf->next = leafNode->next;
    leafNode->next = newLeaf;

    // Handle parent
    if (leafNode == root) {
        Node *newRoot = new Node(false);
        newRoot->keys.push_back(newKey);
        newRoot->children.push_back(leafNode);
        newRoot->children.push_back(newLeaf);
        leafNode->parent = newRoot;  // Set parent for left child
        newLeaf->parent = newRoot;   // Set parent for right child
        this->root = newRoot;
    } else {
        insertInternal(leafNode->parent, newKey,  newLeaf);
    }
}

pair<float, BPlusTree::Node *> BPlusTree::insertInternal(BPlusTree::Node *currNode, float key, BPlusTree::Node *newChild) {
    // Find the location to insert the key
    int i = 0;
    while (i < currNode->keys.size() and key >= currNode->keys[i]) {
        i++;
    }

    currNode->keys.insert(currNode->keys.begin() + i, key);
    currNode->children.insert(currNode->children.begin() + i + 1, newChild);
    newChild->parent = currNode;

    if (currNode->keys.size() > 2 * d) {
        splitInternal(currNode);
    }

    return {key, newChild};
}

void BPlusTree::splitInternal(BPlusTree::Node *internalNode) {
    Node *newInternal = new Node(false);

    int splitIndex = d; // middle index
    float upKey = internalNode->keys[splitIndex];

    newInternal->keys.assign(internalNode->keys.begin() + splitIndex + 1, internalNode->keys.end());
    newInternal->children.assign(internalNode->children.begin() + splitIndex + 1, internalNode->children.end());

    // Need to update parent pointers for all children in newInternal
    for (Node *child : newInternal->children) {
        child->parent = newInternal;
    }

    internalNode->keys.resize(splitIndex);
    internalNode->children.resize(splitIndex + 1);

    // Handle parent
    if (internalNode == this->root) {
        Node *newRoot = new Node(false);
        newRoot->keys.push_back(upKey);
        newRoot->children.push_back(internalNode);
        newRoot->children.push_back(newInternal);
        internalNode->parent = newRoot;
        newInternal->parent = newRoot;

        this->root = newRoot;
    } else {
        insertInternal(internalNode->parent, upKey, newInternal);
    }
}

void BPlusTree::displayTree(Node* node, int level) {
    if (node == nullptr) {
        cout << "Empty tree" << endl;
        return;
    }

    // Print current node
    cout << string(level * 4, ' '); // Indent based on level
    cout << "Level " << level << " [";
    
    // Print keys
    for (int i = 0; i < node->keys.size(); i++) {
        cout << node->keys[i];
        if (i < node->keys.size() - 1) cout << ", ";
    }
    cout << "]";

    // If it's a leaf, print record IDs
    if (node->isLeaf) {
        cout << " Records: [";
        for (int i = 0; i < node->recordIDs.size(); i++) {
            cout << "{";
            for (int j = 0; j < node->recordIDs[i].size(); j++) {
                cout << node->recordIDs[i][j];
                if (j < node->recordIDs[i].size() - 1) cout << ",";
            }
            cout << "}";
            if (i < node->recordIDs.size() - 1) cout << ", ";
        }
        cout << "]";
    }
    cout << endl;

    // Recursively print children
    if (!node->isLeaf) {
        for (Node* child : node->children) {
            displayTree(child, level + 1);
        }
    }
}

vector<float> BPlusTree::traverseLeaves() {
    vector<float> keys;
    
    if (root == nullptr) {
        return keys;
    }

    // Find the leftmost leaf node
    Node* current = root;
    while (!current->isLeaf) {
        current = current->children[0];
    }

    // Traverse through all leaf nodes using the 'next' pointer
    while (current != nullptr) {
        for (float key : current->keys) {
            keys.push_back(key);
        }
        current = current->next;
    }

    return keys;
}

#endif
