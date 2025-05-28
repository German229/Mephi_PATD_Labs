#pragma once
#include <iostream>
using namespace std;

template<typename T>
struct TreeNode {
    T value;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const T& value) : value(value), left(nullptr), right(nullptr) {}
};

template<typename T>
class Tree {
private:
    TreeNode<T>* root;

    TreeNode<T>* insert(TreeNode<T>* node, const T& value) {
        if (node == nullptr) {
            return new TreeNode<T>(value);
        }
        if (node->value > value) {
            node->left = insert(node->left, value);
        } else if (node->value < value) {
            node->right = insert(node->right, value);
        }
        return node;
    }

    TreeNode<T>* remove(TreeNode<T>* node, const T& value) {
        if (node == nullptr) return nullptr;

        if (node->value > value) {
            node->left = remove(node->left, value);
        } else if (node->value < value) {
            node->right = remove(node->right, value);
        } else {
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                return nullptr;
            }
            if (node->left == nullptr) {
                TreeNode<T>* temp = node->right;
                delete node;
                return temp;
            }
            if (node->right == nullptr) {
                TreeNode<T>* temp = node->left;
                delete node;
                return temp;
            }

            TreeNode<T>* minRight = findMin(node->right);
            node->value = minRight->value;
            node->right = remove(node->right, minRight->value);
        }
        return node;
    }

    TreeNode<T>* findMin(TreeNode<T>* node) {
        while (node->left != nullptr)
            node = node->left;
        return node;
    }

    bool find(TreeNode<T>* node, const T& value) const {
        if (node == nullptr) return false;
        if (node->value == value) return true;
        if (node->value > value)
            return find(node->left, value);
        else
            return find(node->right, value);
    }

    void concatHelper(TreeNode<T>* node) {
        if (node == nullptr) return;
        insertEl(node->value);
        concatHelper(node->left);
        concatHelper(node->right);
    }

    template<typename Func>
    void mapHelper(TreeNode<T>* node, Tree<T>& newTree, Func f) const {
        if (node == nullptr) return;
        newTree.insertEl(f(node->value));
        mapHelper(node->left, newTree, f);
        mapHelper(node->right, newTree, f);
    }

    template<typename Predicate>
    void whereHelper(TreeNode<T>* node, Tree<T>& newTree, Predicate pred) const {
        if (node == nullptr) return;
        if (pred(node->value)) {
            newTree.insertEl(node->value);
        }
        whereHelper(node->left, newTree, pred);
        whereHelper(node->right, newTree, pred);
    }

    void destroy(TreeNode<T>* node) {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    void preorder(TreeNode<T>* node) const {
        if (node == nullptr) return;
        cout << node->value << " ";
        preorder(node->left);
        preorder(node->right);
    }

    void inorder(TreeNode<T>* node) const {
        if (node == nullptr) return;
        inorder(node->left);
        cout << node->value << " ";
        inorder(node->right);
    }

    void postorder(TreeNode<T>* node) const {
        if (node == nullptr) return;
        postorder(node->left);
        postorder(node->right);
        cout << node->value << " ";
    }

    void preorderKPL(TreeNode<T>* node) const {
        if (node == nullptr) return;
        cout << node->value << " ";
        preorderKPL(node->right);
        preorderKPL(node->left);
    }

    void postorderPLK(TreeNode<T>* node) const {
        if (node == nullptr) return;
        postorderPLK(node->right);
        postorderPLK(node->left);
        cout << node->value << " ";
    }

    void inorderPKL(TreeNode<T>* node) const {
        if (node == nullptr) return;
        inorderPKL(node->right);
        cout << node->value << " ";
        inorderPKL(node->left);
    }

public:
    Tree() : root(nullptr) {}
    ~Tree() {
        destroy(root);
    }

    void insertEl(const T& value) {
        root = insert(root, value);
    }

    void removeEl(const T& value) {
        root = remove(root, value);
    }

    bool findEl(const T& value) const {
        return find(root, value);
    }

    void concat(Tree<T>& other) {
        concatHelper(other.root);
    }

    template<typename Func>
    Tree<T> map(Func f) const {
        Tree<T> newTree;
        mapHelper(root, newTree, f);
        return newTree;
    }

    template<typename Predicate>
    Tree<T> where(Predicate pred) const {
        Tree<T> newTree;
        whereHelper(root, newTree, pred);
        return newTree;
    }

    void printPreorder() const {
        preorder(root);
        cout << endl;
    }

    void printInorder() const {
        inorder(root);
        cout << endl;
    }

    void printPostorder() const {
        postorder(root);
        cout << endl;
    }

    void printPreorderKPL() const {
        preorderKPL(root);
        cout << endl;
    }

    void printPostorderPLK() const {
        postorderPLK(root);
        cout << endl;
    }

    void printInorderPKL() const {
        inorderPKL(root);
        cout << endl;
    }
};
