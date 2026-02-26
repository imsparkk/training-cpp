#include <iostream>
#include <vector>

using namespace std;

struct Item {
    string name;
    double price;
    int quantity;
};

struct Order {
    int id;
    string customer;
    vector<Item> items;
};

struct Store {
    vector<Order> orders;
};

int main()
{
    std::cout << "Hello World!\n";
}

