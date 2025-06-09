#include "Deque.h"
#include <iostream>

using namespace std;

void print_menu() {
    cout << "\n1. push_back\n";
    cout << "2. push_front\n";
    cout << "3. pop_back\n";
    cout << "4. pop_front\n";
    cout << "5. print\n";
    cout << "6. front\n";
    cout << "7. back\n";
    cout << "8. size\n";
    cout << "9. rotate_left\n";
    cout << "10. rotate_right\n";
    cout << "11. is_sorted\n";
    cout << "0. exit\n";
}

int main() {
    Deque<int> dq;
    int choice, val;

    while (true) {
        print_menu();
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Value: ";
                cin >> val;
                dq.push_back(val);
                break;
            case 2:
                cout << "Value: ";
                cin >> val;
                dq.push_front(val);
                break;
            case 3:
                dq.pop_back();
                break;
            case 4:
                dq.pop_front();
                break;
            case 5:
                cout << "Deque contents: ";
                for (int i = 0; i < dq.size(); ++i)
                    cout << dq[i] << " ";
                cout << "\n";
                break;
            case 6:
                if (!dq.empty())
                    cout << "Front: " << dq.front() << "\n";
                else
                    cout << "Deque is empty.\n";
                break;
            case 7:
                if (!dq.empty())
                    cout << "Back: " << dq.back() << "\n";
                else
                    cout << "Deque is empty.\n";
                break;
            case 8:
                cout << "Size: " << dq.size() << "\n";
                break;
            case 9:
                cout << "Rotate left by: ";
                cin >> val;
                dq.rotate_left(val);
                break;
            case 10:
                cout << "Rotate right by: ";
                cin >> val;
                dq.rotate_right(val);
                break;
            case 11:
                cout << "Is sorted? " << (dq.is_sorted() ? "Yes" : "No") << "\n";
                break;
            case 0:
                return 0;
            default:
                cout << "Invalid option.\n";
        }
    }
}
