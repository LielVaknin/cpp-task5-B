#pragma once

#include <map>
#include <unordered_set>

namespace ariel {
    enum traversal {
        Preorder,
        Inorder,
        Postorder
    };

    template<typename T>
    class BinaryTree {
        struct Node {
            T value;
            Node *left;
            Node *right;
            Node *parent;

            explicit Node(T val, Node *parent = nullptr) : value(val), left(nullptr), right(nullptr), parent(parent) {}
        };

        Node* root;
        std::multimap<T, Node*> nodes;

    public:

        BinaryTree() {
           root = nullptr;
        }

        ~BinaryTree() = default;

        BinaryTree(const BinaryTree &other) {
            if (other.root != nullptr) {
                root = new Node(other.root->value);
                nodes.insert(std::make_pair(root->value, root));
                deepCopy(other.root);
            }
        }

        BinaryTree(BinaryTree &&other) noexcept {
            root = other.root;
            other.root = nullptr;
        }

        BinaryTree<T> &operator=(const BinaryTree<T> &other) {
            if (this == &other) { return *this; }
            if (root != nullptr) {
                delete root;
                root = nullptr;
                nodes.clear();
            }
            this->root = new Node(other.root->value);
            nodes.insert(std::make_pair(this->root->value, this->root));
            deepCopy(other.root);
            return *this;
        }

        BinaryTree &operator=(BinaryTree &&other) noexcept {
            if (root) {
                delete root;
            }
            root = other.root;
            other.root = nullptr;
        }

        void deepCopy(Node *other) {
            if (other->right) {
                add_right(other->value, other->right->value);
                deepCopy(other->right);
            }
            if (other->left) {
                add_left(other->value, other->left->value);
                deepCopy(other->left);
            }
        }

        BinaryTree<T> &add_root(T val) {
            if (root) {
                auto itRemove = nodes.find(root->value);
                nodes.erase(itRemove);
                root->value = val;
                nodes.insert(std::make_pair(root->value, root));
                return *this;
            }
            root = new Node(val);
            nodes.insert(std::make_pair(root->value, root));
            return *this;
        }

        BinaryTree<T> &add_left(T val_exists, T val_to_add) {
            if (nodes.count(val_exists) == 0) {
                throw std::invalid_argument{"Value does not exists in the tree!"};
            }
            auto it = nodes.find(val_exists);
            Node *n = it->second;
            if (n->left == nullptr) {
                n->left = new Node(val_to_add, n);
                nodes.insert(std::make_pair(val_to_add, n->left));
            } else {
                auto itRemove = nodes.find(n->left->value);
                nodes.erase(itRemove);
                n->left->value = val_to_add;
                nodes.insert(std::make_pair(val_to_add, n->left));
            }
            return *this;
        }

        BinaryTree<T> &add_right(T val_exists, T val_to_add) {
            if (nodes.count(val_exists) == 0) {
                throw std::invalid_argument{"Value does not exists in the tree!"};
            }
            auto it = nodes.find(val_exists);
            Node *n = it->second;
            if (n->right == nullptr) {
                n->right = new Node(val_to_add, n);
                nodes.insert(std::make_pair(val_to_add, n->right));
            } else {
                auto itRemove = nodes.find(n->right->value);
                nodes.erase(itRemove);
                n->right->value = val_to_add;
                nodes.insert(std::make_pair(val_to_add, n->right));
            }
            return *this;
        }

        friend std::ostream &operator<<(std::ostream &os, const BinaryTree<T> &bt) {
            return os;
        }

        bool operator<(const BinaryTree &other) const {
            return root->value < other.root->value;
        }

        class iterator {
            Node *curr;
            traversal type;
            std::unordered_set<Node*> visited;
        public:
            explicit iterator(Node* curr, traversal type) : curr(curr), type(type) {}

            T &operator*() const {
                return curr->value;
            }

            T *operator->() const {
                return &(curr->value);
            }

            iterator &postorderTraversal() {
                while (curr && visited.find(curr) == visited.end()) {
                    visited.insert(curr);
                    if (curr->parent && curr->parent->right &&
                        visited.find(curr->parent->right) == visited.end()) {
                        curr = curr->parent->right;
                        if (!curr->left) {
                            while (curr->right) {
                                curr = curr->right;
                            }
                        }
                        while (curr->left) {
                            curr = curr->left;
                        }
                    } else if (curr->parent) {
                        visited.insert(curr);
                        curr = curr->parent;
                    } else {
                        curr = nullptr;
                    }
                    return *this;
                }
                return *this;
            }

            iterator &inorderTraversal() {
                while (curr && visited.find(curr) == visited.end()) {
                    visited.insert(curr);
                    if (curr->left && visited.find(curr->left) == visited.end()) {
                        curr = curr->left;
                    } else if (curr->right && visited.find(curr->right) == visited.end()) {
                        curr = curr->right;
                        while (curr->left) {
                            curr = curr->left;
                        }
                        return *this;
                    }
                    if (curr->parent && visited.find(curr->parent) == visited.end()) {
                        visited.insert(curr);
                        curr = curr->parent;
                    } else if (curr->parent) {
                        while (curr->parent) {
                            if (visited.find(curr->parent) == visited.end()) {
                                curr = curr->parent;
                                break;
                            }
                            curr = curr->parent;

                        }
                        if (curr->parent == nullptr && visited.find(curr) != visited.end()) {
                            curr = nullptr;
                        }
                    } else {
                        curr = nullptr;
                    }
                    return *this;
                }
                return *this;
            }

            iterator &preorderTraversal() {
                while (curr && visited.find(curr) == visited.end()) {
                    visited.insert(curr);
                    if (curr->left && visited.find(curr->left) == visited.end()) {
                        curr = curr->left;
                        return *this;
                    }
                    if (curr->right && visited.find(curr->right) == visited.end()) {
                        curr = curr->right;
                        return *this;
                    }
                    while (curr->parent) {
                        if (visited.find(curr->parent) != visited.end()) {
                            if (curr->parent->right && visited.find(curr->parent->right) == visited.end()) {
                                curr = curr->parent->right;
                                return *this;
                            }
                            curr = curr->parent;
                        }
                    }
                    if (curr->parent == nullptr && visited.find(curr) != visited.end()) {
                        curr = nullptr;
                    }
                    return *this;
                }
                return *this;
            }

            iterator &operator++() {
                switch (type) {
                    case traversal::Preorder:
                        preorderTraversal();
                        break;
                    case traversal::Inorder:
                        inorderTraversal();
                        break;
                    case traversal::Postorder:
                        postorderTraversal();
                        break;
                }
                return *this;
            }

            iterator operator++(int) {
                iterator tmp = *this;
                switch (type) {
                    case traversal::Preorder:
                        preorderTraversal();
                        break;
                    case traversal::Inorder:
                        inorderTraversal();
                        break;
                    case traversal::Postorder:
                        postorderTraversal();
                        break;
                }
                return tmp;
            }

            bool operator==(const iterator &rhs) const {
                return this->curr == rhs.curr;
            }

            bool operator!=(const iterator &rhs) const {
                return this->curr != rhs.curr;
            }
        };

        Node* startPostorder(struct Node *node) {
            while (node && (node->left != nullptr || node->right != nullptr)) {
                if (node->left) {
                    node = node->left;
                } else if (node->right) {
                    node = node->right;
                    while (node->left) {
                        node = node->left;
                    }
                }
            }
            return node;
        }

        iterator begin_preorder() {
            return iterator(this->root, Preorder);
        }

        iterator end_preorder() {
            return iterator(nullptr, Preorder);
        }

        iterator begin_inorder() {
            Node* temp = root;
            while (temp != nullptr && temp->left != nullptr) {
                temp = temp->left;
            }
            return iterator(temp, Inorder);
        }

        iterator end_inorder() {
            return iterator(nullptr, Inorder);
        }

        iterator begin_postorder() {
            Node *temp = startPostorder(this->root);
            return iterator(temp, Postorder);
        }

        iterator end_postorder() {
            return iterator(nullptr, Postorder);
        }

        iterator begin() {
            return begin_inorder();
        }

        iterator end() {
            return end_inorder();
        }
    };
}