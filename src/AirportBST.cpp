#include "AirportBST.h"
#include <queue>
#include <stack>
#include <utility>

AirportBST::AirportBST() : root(nullptr), count(0) {}

AirportBST::~AirportBST() { clear(); }

// Insert: walk down from the root. Go left if the new code is smaller, right if
// it is bigger, and stop at the first empty spot. A while loop (not recursion)
// so a long, chain-shaped tree cannot overflow the call stack
bool AirportBST::insert(const Airport& a) {
    if (root == nullptr) {
        root = new Node(a);
        count++;
        return true;
    }
    Node* cur = root;
    while (true) {
        int cmp = a.code.compare(cur->data.code);
        if (cmp == 0) {
            return false;                      // duplicate key: do not insert
        } else if (cmp < 0) {
            if (cur->left == nullptr) {
                cur->left = new Node(a);
                count++;
                return true;
            }
            cur = cur->left;
        } else {
            if (cur->right == nullptr) {
                cur->right = new Node(a);
                count++;
                return true;
            }
            cur = cur->right;
        }
    }
}

// Delete all nodes with a stack instead of recursion (same reason as insert).
void AirportBST::clear() {
    std::stack<Node*> todo;
    if (root != nullptr) todo.push(root);
    while (!todo.empty()) {
        Node* n = todo.top();
        todo.pop();
        if (n->left != nullptr) todo.push(n->left);
        if (n->right != nullptr) todo.push(n->right);
        delete n;
    }
    root = nullptr;
    count = 0;
}

int AirportBST::size() const { return count; }

// Visit the tree one level at a time (level order). Each node is stored together
// with its depth, so we can track the deepest one (the height) and add up all
// the depths (for the average).
int AirportBST::height() const {
    if (root == nullptr) return -1;
    std::queue<std::pair<Node*, int> > q;
    q.push(std::make_pair(root, 0));
    int maxDepth = 0;
    while (!q.empty()) {
        Node* n = q.front().first;
        int d = q.front().second;
        q.pop();
        if (d > maxDepth) maxDepth = d;
        if (n->left != nullptr) q.push(std::make_pair(n->left, d + 1));
        if (n->right != nullptr) q.push(std::make_pair(n->right, d + 1));
    }
    return maxDepth;
}

double AirportBST::averageDepth() const {
    if (root == nullptr) return 0.0;
    std::queue<std::pair<Node*, int> > q;
    q.push(std::make_pair(root, 0));
    long long total = 0;
    while (!q.empty()) {
        Node* n = q.front().first;
        int d = q.front().second;
        q.pop();
        total += d;
        if (n->left != nullptr) q.push(std::make_pair(n->left, d + 1));
        if (n->right != nullptr) q.push(std::make_pair(n->right, d + 1));
    }
    return (double)total / count;
}

// Inorder = left subtree, node, right subtree (gives sorted order for BST).
// Done with a stack: go as far left as possible, take the node on top, then
// move to its right child and repeat
std::vector<std::string> AirportBST::inorderCodes() const {
    std::vector<std::string> result;
    std::stack<Node*> st;
    Node* cur = root;
    while (cur != nullptr || !st.empty()) {
        while (cur != nullptr) {
            st.push(cur);
            cur = cur->left;
        }
        cur = st.top();
        st.pop();
        result.push_back(cur->data.code);
        cur = cur->right;
    }
    return result;
}

// Searches for a code. Fills 'path' with every code compared along the way
// (path.size() is the comparison count). Returns true if the code was found.
bool AirportBST::find(const std::string& code, Airport& result, std::vector<std::string>& path) const {
    path.clear();
    Node* cur = root;
    while (cur != nullptr) {
        path.push_back(cur->data.code);
        int cmp = code.compare(cur->data.code);
        if (cmp == 0) {
            result = cur->data;
            return true;
        }
        cur = (cmp < 0) ? cur->left : cur->right;
    }
    return false;
}

// Finds the airport with the smallest code (keep going left). False if the tree is empty.
bool AirportBST::minAirport(Airport& result) const {
    if (root == nullptr) return false;
    Node* cur = root;
    while (cur->left != nullptr) cur = cur->left;
    result = cur->data;
    return true;
}

// Finds the airport with the largest code (keep going right). False if the tree is empty.
bool AirportBST::maxAirport(Airport& result) const {
    if (root == nullptr) return false;
    Node* cur = root;
    while (cur->right != nullptr) cur = cur->right;
    result = cur->data;
    return true;
}

// Deletes a code using the three cases: D1 leaf, D2 one child, D3 two children
// (D3 copies in the inorder successor, then removes the successor node).
// Returns false if the code is not in the tree; 'info' describes the case used.
bool AirportBST::remove(const std::string& code, std::string& info) {
    Node* parent = nullptr;
    Node* cur = root;
    while (cur != nullptr && cur->data.code != code) {
        parent = cur;
        cur = (code < cur->data.code) ? cur->left : cur->right;
    }
    if (cur == nullptr) return false;

    bool wasRoot = (cur == root);
    if (cur->left != nullptr && cur->right != nullptr) {
        Node* succParent = cur;
        Node* succ = cur->right;
        while (succ->left != nullptr) {
            succParent = succ;
            succ = succ->left;
        }
        info = "D3: two children, replaced by successor " + succ->data.code;
        cur->data = succ->data;
        if (succParent->left == succ) succParent->left = succ->right;
        else succParent->right = succ->right;
        delete succ;
    } else {
        Node* child = (cur->left != nullptr) ? cur->left : cur->right;
        if (child == nullptr) info = "D1: leaf";
        else if (cur->left != nullptr) info = "D2: only a left child, " + child->data.code;
        else info = "D2: only a right child, " + child->data.code;
        if (parent == nullptr) root = child;
        else if (parent->left == cur) parent->left = child;
        else parent->right = child;
        delete cur;
    }
    count--;
    if (wasRoot) info += " (root)";
    return true;
}

// Checks the BST property with the (lo, hi) range method: every code must be
// strictly between the limits set by its ancestors.
bool AirportBST::validate() const {
    struct Item {
        Node* node;
        const std::string* lo;
        const std::string* hi;
    };
    std::stack<Item> st;
    if (root != nullptr) st.push(Item{root, nullptr, nullptr});
    while (!st.empty()) {
        Item it = st.top();
        st.pop();
        const std::string& c = it.node->data.code;
        if (it.lo != nullptr && !(*it.lo < c)) return false;
        if (it.hi != nullptr && !(c < *it.hi)) return false;
        if (it.node->left != nullptr) st.push(Item{it.node->left, it.lo, &c});
        if (it.node->right != nullptr) st.push(Item{it.node->right, &c, it.hi});
    }
    return true;
}

// Returns the top 'levels' levels of the tree, level by level, left to right.
// An empty spot is shown as "-" so the shape of the tree is visible.
std::vector<std::vector<std::string> > AirportBST::levelOrder(int levels) const {
    std::vector<std::vector<std::string> > result;
    std::vector<Node*> current;
    current.push_back(root);
    for (int level = 0; level < levels; level++) {
        std::vector<std::string> line;
        std::vector<Node*> next;
        int n = (int)current.size();
        for (int i = 0; i < n; i++) {
            if (current[i] == nullptr) {
                line.push_back("-");
                next.push_back(nullptr);
                next.push_back(nullptr);
            } else {
                line.push_back(current[i]->data.code);
                next.push_back(current[i]->left);
                next.push_back(current[i]->right);
            }
        }
        result.push_back(line);
        current = next;
    }
    return result;
}

// Returns every airport in alphabetical code order (inorder walk).
std::vector<Airport> AirportBST::inorderAirports() const {
    std::vector<Airport> result;
    std::stack<Node*> st;
    Node* cur = root;
    while (cur != nullptr || !st.empty()) {
        while (cur != nullptr) {
            st.push(cur);
            cur = cur->left;
        }
        cur = st.top();
        st.pop();
        result.push_back(cur->data);
        cur = cur->right;
    }
    return result;
}

// Returns every code in preorder: a node first, then its left subtree, then its right.
// Inserting the codes in this order rebuilds exactly the same tree.
std::vector<std::string> AirportBST::preorderCodes() const {
    std::vector<std::string> result;
    std::stack<Node*> st;
    if (root != nullptr) st.push(root);
    while (!st.empty()) {
        Node* n = st.top();
        st.pop();
        result.push_back(n->data.code);
        if (n->right != nullptr) st.push(n->right);
        if (n->left != nullptr) st.push(n->left);
    }
    return result;
}

// Returns every code in level order (top level first, each level left to right).
std::vector<std::string> AirportBST::levelOrderCodes() const {
    std::vector<std::string> result;
    std::queue<Node*> q;
    if (root != nullptr) q.push(root);
    while (!q.empty()) {
        Node* n = q.front();
        q.pop();
        result.push_back(n->data.code);
        if (n->left != nullptr) q.push(n->left);
        if (n->right != nullptr) q.push(n->right);
    }
    return result;
}

// Collects every airport with from <= code <= to, in sorted order, and returns how
// many nodes were visited. A subtree is skipped when it cannot hold a code in the
// range: go left only if from < code, and go right only if code < to.
int AirportBST::range(const std::string& from, const std::string& to, std::vector<Airport>& result) const {
    result.clear();
    int visited = 0;
    std::stack<Node*> st;
    Node* cur = root;
    while (cur != nullptr || !st.empty()) {
        while (cur != nullptr) {
            visited++;
            st.push(cur);
            cur = (from < cur->data.code) ? cur->left : nullptr;
        }
        cur = st.top();
        st.pop();
        if (!(cur->data.code < from) && !(to < cur->data.code)) result.push_back(cur->data);
        cur = (cur->data.code < to) ? cur->right : nullptr;
    }
    return visited;
}

// Returns true if the code is in the tree (a search that does not record the path).
bool AirportBST::contains(const std::string& code) const {
    Node* cur = root;
    while (cur != nullptr) {
        int cmp = code.compare(cur->data.code);
        if (cmp == 0) return true;
        cur = (cmp < 0) ? cur->left : cur->right;
    }
    return false;
}

// Builds the subtree for a[lo..hi]: the middle element becomes the root, then the
// same rule is applied to the left half and to the right half.
AirportBST::Node* AirportBST::buildRange(const std::vector<Airport>& a, int lo, int hi) {
    if (lo > hi) return nullptr;
    int mid = (lo + hi) / 2;
    Node* n = new Node(a[mid]);
    n->left = buildRange(a, lo, mid - 1);
    n->right = buildRange(a, mid + 1, hi);
    return n;
}

// Replaces the tree with a balanced tree built from airports already sorted by code.
void AirportBST::buildBalanced(const std::vector<Airport>& sortedAirports) {
    clear();
    int n = (int)sortedAirports.size();
    root = buildRange(sortedAirports, 0, n - 1);
    count = n;
}

// Rebuilds the tree as a balanced tree holding the same airports.
void AirportBST::rebalance() {
    std::vector<Airport> all = inorderAirports();
    buildBalanced(all);
}