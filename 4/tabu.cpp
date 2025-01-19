#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <numeric>
#include <climits>

using namespace std;

struct Task {
    int id;
    vector<int> temp;
    int temp_sum;
};

int calculate_cmax(const vector<Task>& data, const vector<int>& order) {
    int M = data[0].temp.size();
    int cmax = 0;
    vector<int> prev(M, 0);
    for (int t_index : order) {
        int t = 0;
        for (int m = 0; m < M; m++) {
            t = max(t, prev[m]) + data[t_index].temp[m];
            prev[m] = t;
            cmax = t;
        }
    }
    return cmax;
}

vector<int> swap_random(const vector<int>& order, int i) {
    vector<int> new_order = order;
    int index1 = i;
    int index2;
    do {
        index2 = rand() % order.size();
    } while (index1 == index2);
    swap(new_order[index1], new_order[index2]);
    return new_order;
}

string order_to_string(const vector<int>& ord) {
    stringstream ss;
    for (int o : ord) {
        ss << o << ",";
    }
    return ss.str();
}

void add_move_tabu_list(vector<pair<int, int>>& tabu_list, const pair<int, int>& move, int tabu_tenure) {
    tabu_list.push_back(move);
    if (tabu_list.size() > tabu_tenure) {
        tabu_list.erase(tabu_list.begin());
    }
}

bool is_move_tabu(const vector<pair<int, int>>& tabu_list, const pair<int, int>& move) {
    return find(tabu_list.begin(), tabu_list.end(), move) != tabu_list.end() ||
           find(tabu_list.begin(), tabu_list.end(), make_pair(move.second, move.first)) != tabu_list.end();
}

vector<int> tabu_search(const vector<Task>& data, int iterations, int tabu_tenure, const string& filename) {
    ofstream result_file(filename, ofstream::out);

    vector<int> order(data.size());
    iota(order.begin(), order.end(), 0);

    int current_cmax = calculate_cmax(data, order);
    int best_cmax = current_cmax;
    vector<int> best_order = order;

    vector<pair<int, int>> tabu_list;

    for (int i = 0; i < iterations; i++) {
        int best_neighbor_cmax = INT_MAX;
        vector<int> best_neighbor_order = order;
        pair<int, int> best_move;

        for (size_t j = 0; j < order.size(); j++) {
            vector<int> new_order = swap_random(order, j);
            pair<int, int> move = {order[j], new_order[j]};

            if (!is_move_tabu(tabu_list, move)) {
                int new_cmax = calculate_cmax(data, new_order);
                if (new_cmax < best_neighbor_cmax) {
                    best_neighbor_cmax = new_cmax;
                    best_neighbor_order = new_order;
                    best_move = move;
                }
            }
        }

        if (best_neighbor_order != order) {
            add_move_tabu_list(tabu_list, best_move, tabu_tenure);
        }

        order = best_neighbor_order;
        current_cmax = best_neighbor_cmax;

        if (current_cmax < best_cmax) {
            best_cmax = current_cmax;
            best_order = order;
        }

        result_file << i << ", " << current_cmax << endl;
    }
    result_file.close();
    return best_order;
}

int main() {
    string filepath = "neh.data.txt";
    ifstream file(filepath);
    vector<vector<Task>> datasets;
    string line;

    if (!file.is_open()) {
        cerr << "Failed to open file: " << filepath << endl;
        return 1;
    }

    vector<Task> current_dataset;
    bool save_data = false;
    int counter = 0;

    while (getline(file, line)) {
        if (line.empty()) {
            save_data = false;
            if (!current_dataset.empty()) {
                datasets.push_back(current_dataset);
                current_dataset.clear();
            }
            continue;
        }

        if (line.find("data.") != string::npos) {
            save_data = true;
            counter = 0;
        } else {
            if (save_data) {
                if (counter == 0) {
                    counter++;
                    continue;
                }
                istringstream iss(line);
                int num;
                int temp_sum = 0;
                vector<int> task_temp;
                while (iss >> num) {
                    temp_sum += num;
                    task_temp.push_back(num);
                }
                current_dataset.push_back({counter, task_temp, temp_sum});
                counter++;
            }
        }
    }

    if (!current_dataset.empty()) {
        datasets.push_back(current_dataset);
    }

    file.close();

    int data_from = 100;
    int data_to = 100;

    cout << "Tabu Search Results" << endl;

    chrono::duration<double> total_time = chrono::duration<double>::zero();

    int iterations = 1000;  // Liczba iteracji
    int tabu_tenure = 10;    // Rozmiar tablicy tabu

    int best_overall_cmax = INT_MAX;

    for (int i = data_from; i <= data_to; i++) {
        auto start = chrono::high_resolution_clock::now();
        vector<int> result = tabu_search(datasets[i], iterations, tabu_tenure, "results_tabu.csv");
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        int cmax = calculate_cmax(datasets[i], result);
        if (cmax < best_overall_cmax) {
            best_overall_cmax = cmax;
        }

        total_time += duration;
        cout << "data." << i << ": Cmax: " << cmax << ", Time: " << duration.count() << "s" << endl;
    }
    ofstream result_file("results_tabu.csv", ofstream::out | ofstream::app);
    result_file << iterations << ", " << best_overall_cmax << endl;
    result_file.close();
    return 0;
}
