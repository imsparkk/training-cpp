#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace std;

#define MAX_DISCOUNT 20
#define SERVICE_PERCENT 10
#define MIN_PRICE 0
#define MAX_PRICE 1000000

#if MIN_PRICE < 0 || MAX_PRICE > 1000000
#error "error"
#endif

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

double calculateOrderSum(Order order) {
    double sum = 0;
    for (Item item : order.items) {
        sum += item.price * item.quantity;
    }
    return sum + (sum * SERVICE_PERCENT / 100);
}

void printOrder(Order order) {
    cout << "Order ID: " << order.id << endl;
    cout << "Customer: " << order.customer << endl;
    for (Item item : order.items) {
        cout << "  " << item.name << " - " << item.price << " x " << item.quantity << endl;
    }
}

void printOrder(Order order, double discount) {
    double sum = calculateOrderSum(order);
    double discountedSum = sum - (sum * discount / 100);
    printOrder(order);
    cout << "Total " << discount << "% discount: " << discountedSum << endl;
}

double operator+(Order order, double discount) {
    double sum = calculateOrderSum(order);
    return sum - (sum * discount / 100);
}

void removeEmptyOrders(Store& store) {
    for (int i = store.orders.size() - 1; i >= 0; i--) {
        if (store.orders[i].items.size() == 0) {
            store.orders.erase(store.orders.begin() + i);
        }
    }
}

bool compareBySum(Order a, Order b) {
    return calculateOrderSum(a) < calculateOrderSum(b);
}

bool compareByName(Order a, Order b) {
    return a.customer < b.customer;
}

bool compareByQuantity(Order a, Order b) {
    int totalA = 0;
    int totalB = 0;
    for (Item item : a.items) totalA += item.quantity;
    for (Item item : b.items) totalB += item.quantity;
    return totalA < totalB;
}

void sortOrders(Store& store, bool (*compare)(Order, Order)) {
    for (int i = 0; i < store.orders.size() - 1; i++) {
        for (int j = 0; j < store.orders.size() - i - 1; j++) {
            if (compare(store.orders[j + 1], store.orders[j])) {
                Order temp = store.orders[j];
                store.orders[j] = store.orders[j + 1];
                store.orders[j + 1] = temp;
            }
        }
    }
}

Item getMostPopularItem(Store store) {
    vector<string> itemNames;
    vector<int> itemCounts;

    for (Order order : store.orders) {
        for (Item item : order.items) {
            bool found = false;
            for (int i = 0; i < itemNames.size(); i++) {
                if (itemNames[i] == item.name) {
                    itemCounts[i] += item.quantity;
                    found = true;
                    break;
                }
            }
            if (!found) {
                itemNames.push_back(item.name);
                itemCounts.push_back(item.quantity);
            }
        }
    }

    int maxIndex = 0;
    for (int i = 1; i < itemCounts.size(); i++) {
        if (itemCounts[i] > itemCounts[maxIndex]) {
            maxIndex = i;
        }
    }

    Item result;
    result.name = itemNames[maxIndex];
    result.price = 0;
    result.quantity = itemCounts[maxIndex];
    return result;
}

vector<vector<double>> createSalesMatrix(Store store) {
    vector<string> allItems;

    for (Order order : store.orders) {
        for (Item item : order.items) {
            bool found = false;
            for (string name : allItems) {
                if (name == item.name) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                allItems.push_back(item.name);
            }
        }
    }

    vector<vector<double>> matrix;
    for (Order order : store.orders) {
        vector<double> row;
        for (string itemName : allItems) {
            double value = 0;
            for (Item item : order.items) {
                if (item.name == itemName) {
                    value = item.price * item.quantity;
                    break;
                }
            }
            row.push_back(value);
        }
        matrix.push_back(row);
    }

    return matrix;
}

vector<Order> findOrdersBySubstring(Store store, string substr) {
    vector<Order> result;
    for (Order order : store.orders) {
        for (Item item : order.items) {
            if (item.name.find(substr) != string::npos) {
                result.push_back(order);
                break;
            }
        }
    }
    return result;
}

void loadOrdersFromFile(Store& store, string filename) {
    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        Order order;
        string itemData;

        ss >> order.id >> order.customer;

        while (ss >> itemData) {
            Item item;
            size_t pos1 = itemData.find(',');
            size_t pos2 = itemData.find(',', pos1 + 1);

            item.name = itemData.substr(0, pos1);
            item.price = stod(itemData.substr(pos1 + 1, pos2 - pos1 - 1));
            item.quantity = stoi(itemData.substr(pos2 + 1));

            order.items.push_back(item);
        }

        store.orders.push_back(order);
    }

    file.close();
}

void saveOrdersToFile(Store store, string filename) {
    ofstream file(filename);

    for (Order order : store.orders) {
        file << order.id << " " << order.customer;
        for (Item item : order.items) {
            file << " " << item.name << "," << item.price << "," << item.quantity;
        }
        file << endl;
    }

    file.close();
}

void printStatistics(Store store) {
    if (store.orders.size() == 0) {
        cout << "no orders" << endl;
        return;
    }

    double totalRevenue = 0;
    double maxOrderSum = 0;
    string bestCustomer;
    map<string, double> customerSpending;

    for (Order order : store.orders) {
        double sum = calculateOrderSum(order);
        totalRevenue += sum;

        if (sum > maxOrderSum) {
            maxOrderSum = sum;
        }

        customerSpending[order.customer] += sum;
    }

    double maxSpending = 0;
    for (auto pair : customerSpending) {
        if (pair.second > maxSpending) {
            maxSpending = pair.second;
            bestCustomer = pair.first;
        }
    }

    double averageCheck = totalRevenue / store.orders.size();

    cout << "total: " << totalRevenue << endl;
    cout << "avg: " << averageCheck << endl;
    cout << "expensive: " << maxOrderSum << endl;
    cout << "best: " << bestCustomer << " spent " << maxSpending << endl;
}

int main()
{
}