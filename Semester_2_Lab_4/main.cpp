#include <iostream>
#include <string>
#include <ctime>
using namespace std;

struct PersonID {
    int series;
    int number;

    bool operator<(const PersonID& other) const {
        return tie(series, number) < tie(other.series, other.number);
    }
    bool operator==(const PersonID& other) const {
        return tie(series, number) == tie(other.series, other.number);
    }
};


class Person {
protected:
    PersonID id;
    string firstName;
    string middleName;
    string lastName;
    time_t birthDate;

public:
    Person(std::string fn, std::string mn, std::string ln, time_t bd, PersonID pid)
        : firstName(fn), middleName(mn), lastName(ln), birthDate(bd), id(pid) {}

    virtual ~Person() {}

    virtual std::string GetFullName() const {
        return lastName + " " + firstName + " " + middleName;
    }

    PersonID GetID() const { return id; }

    virtual bool operator<(const Person& other) const {
        return id < other.id;
    }

    virtual bool operator==(const Person& other) const {
        return id == other.id;
    }
};


class Student : Person {

};

template<typename T>
struct TreeNode {
    T value;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const T& value) : value(value), left(nullptr), right(nullptr) {};
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
        }
        else if (node->value < value) {
            node->right = insert(node->right, value);
        }

        return node;
    }

    TreeNode<T>* remove(TreeNode<T>* node, const T& value) {
        if (node == nullptr) {
            return nullptr;
        }
        if (node->value > value) {
            node->left = remove(node->left, value);
        }
        else if (node->value < value) {
            node->right = remove(node->right, value);
        }
        else {
            if (node->right == nullptr && node->left == nullptr) {
                delete node;
                return nullptr;
            }
            if (node->left == nullptr) {
                TreeNode<T>* temp = node->right;
                delete node;
                return temp;
            }
            else if (node->right == nullptr) {
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


    bool find(TreeNode<T>* node, const T& value) {
        if (node == nullptr) {
            return false;
        }
        if (node->value == value) {
            return true;
        }
        if (node->value > value) {
            return find(node->left, value);
        }
        else {
            return find(node->right, value);
        }
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

public:
    Tree() : root(nullptr) {};
    ~Tree() {
        destroy(root);
    }

    void insertEl(const T& value) {
        root = insert(root, value);
    }
    void removeEl(const T& value) {
        root = remove(root, value);
    }

    bool findEl(const T& value) {
        return find(root, value);
    }

    void destroy(TreeNode<T>* node) {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }


    // TODO КЛП (Корень, Лево, Право)
    void preorder(TreeNode<T>* node) {
        if (node == nullptr) return;
        cout << node-> value << " ";
        preorder(node->left);
        preorder(node->right);
    }

    // TODO ЛКП (Лево, Корень, Право)
    void inorder(TreeNode<T>* node) {
        if (node == nullptr) return;
        inorder(node->left);
        cout << node->value << " ";
        inorder(node->right);
    }

    // TODO ЛПК (Лево, Право, Корень)
    void postorder(TreeNode<T>* node) {
        if (node == nullptr) return;
        postorder(node->left);
        postorder(node->right);
        cout << node->value << " ";
    }


    // TODO КПЛ (Корень, Право, Лево)
    void preorderKPL(TreeNode<T>* node) {
        if (node == nullptr) return;
        cout << node->value << " ";
        preorderKPL(node->right);
        preorderKPL(node->left);
    }

    // TODO ПЛК (Право, Лево, Корень)
    void postorderPLK(TreeNode<T>* node) {
        if (node == nullptr) return;
        postorderPLK(node->right);
        postorderPLK(node->left);
        cout << node->value << " ";
    }

    // TODO ПКЛ (Право, Корень, Лево)
    void inorderPKL(TreeNode<T>* node) {
        if (node == nullptr) return;
        inorderPKL(node->right);
        cout << node->value << " ";
        inorderPKL(node->left);
    }


    void printPreorder() {
        preorder(root);
        cout << endl;
    }

    void printInorder() {
        inorder(root);
        cout << endl;
    }

    void printPostorder() {
        postorder(root);
        cout << endl;
    }

    void printPreorderKPL() {
        preorderKPL(root);
        cout << endl;
    }

    void printPostorderPLK() {
        postorderPLK(root);
        cout << endl;
    }

    void printInorderPKL() {
        inorderPKL(root);
        cout << endl;
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


    template <typename Predicate>
    Tree<T> where(Predicate pred) const {
        Tree<T> newTree;
        whereHelper(root, newTree, pred);
        return newTree;
    }









};

int main() {
    Tree<int> treeA;
    Tree<int> treeB;
    int choice, value;

    while (true) {
        cout << "\n=== MENU ===\n";
        cout << "1. Insert into Tree A\n";
        cout << "2. Insert into Tree B\n";
        cout << "3. Remove from Tree A\n";
        cout << "4. Find in Tree A\n";
        cout << "5. Print Tree A (KLP)\n";
        cout << "6. Print Tree A (LKP)\n";
        cout << "7. Print Tree A (LPK)\n";
        cout << "8. Print Tree A (KPL)\n";
        cout << "9. Print Tree A (PLK)\n";
        cout << "10. Print Tree A (PKL)\n";
        cout << "11. Concat Tree B into Tree A\n";
        cout << "12. Print Tree B (LKP)\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 0:
                return 0;
            case 1:
                cout << "Value to insert into Tree A: ";
                cin >> value;
                treeA.insertEl(value);
                break;
            case 2:
                cout << "Value to insert into Tree B: ";
                cin >> value;
                treeB.insertEl(value);
                break;
            case 3:
                cout << "Value to remove from Tree A: ";
                cin >> value;
                treeA.removeEl(value);
                break;
            case 4:
                cout << "Value to find in Tree A: ";
                cin >> value;
                if (treeA.findEl(value))
                    cout << "Found.\n";
                else
                    cout << "Not found.\n";
                break;
            case 5:
                cout << "Tree A (Preorder KLP): ";
                treeA.printPreorder();
                break;
            case 6:
                cout << "Tree A (Inorder LKP): ";
                treeA.printInorder();
                break;
            case 7:
                cout << "Tree A (Postorder LPK): ";
                treeA.printPostorder();
                break;
            case 8:
                cout << "Tree A (Preorder KPL): ";
                treeA.printPreorderKPL();
                break;
            case 9:
                cout << "Tree A (Postorder PLK): ";
                treeA.printPostorderPLK();
                break;
            case 10:
                cout << "Tree A (Inorder PKL): ";
                treeA.printInorderPKL();
                break;
            case 11:
                treeA.concat(treeB);
                cout << "Tree B has been concatenated into Tree A.\n";
                break;
            case 12:
                cout << "Tree B (Inorder LKP): ";
                treeB.printInorder();
                break;
            default:
                cout << "Invalid choice.\n";
        }
    }

    return 0;
}
