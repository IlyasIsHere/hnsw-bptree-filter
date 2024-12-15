#include <vector>
#include <utility>

using namespace std;


class BPlusTree {
private:
    struct Node {
        bool isLeaf;
        vector<float> keys;
        vector<vector<int>> recordIDs; // i-th element is a vector of the IDs of the vector data points having key == keys[i] (to handle duplicates) (only used for leaf nodes)
        vector<Node *> children; // This would have a length of keys.size() + 1
        Node *next; // Used only for leaf nodes (for range queries)

        Node(bool isLeaf = false) {
            this->isLeaf = isLeaf;
            next = nullptr;
            // TODO: should the recordID be init?
        }
    };

    Node *root;
    int d; // The order of the B+Tree (the max number of keys in a node is 2*d)

    Node *getLeaf(float key); // Helper function to get the leaf where the "key" should be


public:
    BPlusTree(int order) : d(order), root(nullptr) {}

    // Inserts a new data point
    void insert(float key, int recordID);

    // Searches for data points by a key and returns a vector of their IDs
    vector<int> search(float key);

    // Removes all data points by a key
    void remove(float key);

    // Performs a range query using a lower and upper key
    vector<int> rangeSearch(float lower, float upper);
};

BPlusTree::Node *BPlusTree::getLeaf(float key) {
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
        if (key == leafPtr->keys[i]) {
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

        auto it = lower_bound(leafNode->keys.begin(), leafNode->keys.end(), key);
        int pos = it - leafNode->keys.begin();

        if (it != leafNode->keys.end() and *it == key) {
            // If the key already exists
            leafNode->recordIDs[pos].push_back(recordID);
        } else {
            leafNode->keys.insert(it, key);
            leafNode->recordIDs.insert(leafNode->recordIDs.begin() + pos, {recordID});
        }

        // Check if the leaf node is overfull
        if (leafNode->keys.size() > 2 * d) {
            splitLeaf(leafNode); // TODO: implement splitLeaf
        }
    }
}



