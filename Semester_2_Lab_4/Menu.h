#pragma once
#include <iostream>
#include "Tree.h"
#include "Student.h"
#include "Teacher.h"

using namespace std;

void runMenu() {
    Tree<int> treeA;
    Tree<int> treeB;
    Tree<Student> studentTree;
    Tree<Teacher> teacherTree;
    int choice, value;

    while (true) {
        cout << "\n=== MENU ===\n";
        cout << "1. Insert into Tree A (int)\n";
        cout << "2. Insert into Tree B (int)\n";
        cout << "3. Remove from Tree A (int)\n";
        cout << "4. Find in Tree A (int)\n";
        cout << "5. Print Tree A (Preorder KLP)\n";
        cout << "6. Print Tree A (Inorder LKP)\n";
        cout << "7. Print Tree A (Postorder LPK)\n";
        cout << "8. Print Tree A (Preorder KPL)\n";
        cout << "9. Print Tree A (Postorder PLK)\n";
        cout << "10. Print Tree A (Inorder PKL)\n";
        cout << "11. Concat Tree B into Tree A (int)\n";
        cout << "12. Print Tree B (Inorder LKP)\n";
        cout << "13. Map Tree A (x2 all elements)\n";
        cout << "14. Where Tree A (keep only even elements)\n";
        cout << "15. Insert Student into Student Tree\n";
        cout << "16. Insert Teacher into Teacher Tree\n";
        cout << "17. Print Student Tree (Inorder)\n";
        cout << "18. Print Teacher Tree (Inorder)\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 0:
                return;
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
                cout << (treeA.findEl(value) ? "Found\n" : "Not Found\n");
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
            case 13: {
                Tree<int> mappedTree = treeA.map([](int x) { return x * 2; });
                cout << "Mapped Tree A (x2), Inorder: ";
                mappedTree.printInorder();
                break;
            }
            case 14: {
                Tree<int> filteredTree = treeA.where([](int x) { return x % 2 == 0; });
                cout << "Filtered Tree A (only even elements), Inorder: ";
                filteredTree.printInorder();
                break;
            }


            case 15: {
                PersonID pid;
                string fn, mn, ln, group;
                time_t bd = time(nullptr);
                cout << "Enter Student series and number: ";
                cin >> pid.series >> pid.number;
                cout << "Enter Student first name: ";
                cin >> fn;
                cout << "Enter Student middle name: ";
                cin >> mn;
                cout << "Enter Student last name: ";
                cin >> ln;
                cout << "Enter Student group: ";
                cin >> group;
                Student s(fn, mn, ln, bd, pid, group);
                studentTree.insertEl(s);
                break;
            }


            case 16: {
                PersonID pid;
                string fn, mn, ln, dept;
                time_t bd = time(nullptr);
                cout << "Enter Teacher series and number: ";
                cin >> pid.series >> pid.number;
                cout << "Enter Teacher first name: ";
                cin >> fn;
                cout << "Enter Teacher middle name: ";
                cin >> mn;
                cout << "Enter Teacher last name: ";
                cin >> ln;
                cout << "Enter Teacher department: ";
                cin >> dept;
                Teacher t(fn, mn, ln, bd, pid, dept);
                teacherTree.insertEl(t);
                break;
            }

            case 17:
                cout << "Student Tree (Inorder): ";
                studentTree.printInorder();
                break;
            case 18:
                cout << "Teacher Tree (Inorder): ";
                teacherTree.printInorder();
                break;

            default:
                cout << "Invalid choice.\n";
        }
    }
}
