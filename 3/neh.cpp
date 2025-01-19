#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <chrono>
#include <limits>

using namespace std;

struct Task {
    int id;
    vector<int> temp;
    int temp_sum;
};

vector<int> sort_task_order(const vector<Task>& data) {
    vector<Task> sorted_data = data;
    stable_sort(sorted_data.begin(), sorted_data.end(), [](const Task& a, const Task& b) {
        return a.temp_sum > b.temp_sum;
    });

    vector<int> order;

    transform(sorted_data.begin(), sorted_data.end(), back_inserter(order), [](const Task& t) {
        return t.id - 1;
    });

    return order;
}

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

vector<int> NEH(const vector<Task>& data) {
    vector<int> input_order = sort_task_order(data);
    vector<int> order;
    int N_tasks = data.size();
    int M = data[0].temp.size();
    int index = 0;

    for (int t_index : input_order) {
        int N = order.size();
        int C_max = numeric_limits<int>::max();
        for (int i = 0; i < N + 1; i++) {
            vector<int> new_order = order;
            new_order.insert(new_order.begin() + i, t_index);
            int c = calculate_cmax(data, new_order);
            if (c < C_max) {
                C_max = c;
                index = i;
            }
        }
        order.insert(order.begin() + index, t_index);
    }
    return order;
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

    int data_from = 0;
    int data_to = 120;
    cout << "NEH Results" << endl;

    chrono::duration<double> total_time = chrono::duration<double>::zero();
    for (int i = data_from; i <= data_to; i++) {
        cout << "data." << i << ": Cmax: ";
        auto start = chrono::high_resolution_clock::now();
        vector<int> result = NEH(datasets[i]);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        total_time += duration;
        cout << calculate_cmax(datasets[i], result) << " ";
        cout << "Czas: " << duration.count() << endl;
    }
    cout << "Czas dzialania programu: " << total_time.count() << " s" << endl;

    return 0;
}
