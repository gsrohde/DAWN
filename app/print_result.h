#include <iostream>
#include <iomanip>
#include <framework/state_map.h>

using std::cout;
using std::endl;
using std::setw;

void print_result(const state_vector_map &result) {

    cout.precision(4);
    for (auto item : result) {
        cout << setw(22) << item.first << "   ";
    }
    cout << endl;

    // find the size of the vector values
    if (result.empty()) {
        cout << "Can't print an empty container" << endl;
        exit(1);
    }

    auto first_element = result.begin();
    auto first_element_vector = first_element->second;
    int size = first_element_vector.size();

    try {
        for (int i = 0; i < size; ++i) {
            for (auto item : result) {
                cout << setw(22) << item.second[i] << "   ";
            }
            cout << endl;
        }
    } catch(std::exception& err) {
        cout << "error: " << err.what() << endl;
        cout << "keys:" << endl;
        for (auto p : result) {
            cout << p.first << endl;
        }
    }
}
