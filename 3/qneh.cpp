#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <limits>
#include <algorithm>
#include <vector>

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

vector<int> get_matrix_col(const vector<vector<int>>& matrix, int col){
    int M = matrix.size();
    vector<int> column(M, 0);
    for (int i = 0; i < M; ++i){
        column[i] = matrix[i][col];
    }
    return column;
}

void propagate_forward(const vector<Task>& data,
                   const vector<int>& order,
                   vector<vector<int>>& forward,
                   int k=0){
    int M = data[0].temp.size();
    vector<int> prev(M, 0);
    if (k != 0){
        prev = get_matrix_col(forward, order[k-1]);
    }
    for (int i = k; i < order.size(); i++) {
        int t = 0;
        for (int m = 0; m < M; m++) {
            t = max(t, prev[m]) + data[order[i]].temp[m];
            forward[m][order[i]] = t;
            prev[m] = t;
        }
    }
}

void propagate_backward(const vector<Task>& data,
                    const vector<int>& order,
                    vector<vector<int>>& backward,
                    int k=-1){
    if (order.size() == 0) {
        return;
    }
    int M = data[0].temp.size();
    vector<int> prev(M, 0);
    if (k != order.size()-1){
        prev = get_matrix_col(backward, order[k+1]);
    }
    for (int i = k; i >= 0; i--) {
        int t = 0;
        for (int m = M-1; m >= 0; m--) {
            t = max(t, prev[m]) + data[order[i]].temp[m];
            backward[m][order[i]] = t;
            prev[m] = t;
        }
    }
}

int getCmax(const Task& task,
            const vector<vector<int>>& forward,
            const vector<vector<int>>& backward,
            int k,
            const vector<int>& order){
    int t = 0;
    int cmax = 0;
    int N = order.size();
    int M = task.temp.size();
    vector<int> task_table(M, 0);
    vector<int> prev;
    if (k != 0){
        prev = get_matrix_col(forward, order[k-1]);
    } else {
        prev = vector<int>(M, 0);
    }
    for (int m = 0; m < M; m++) {
        t = max(t, prev[m]) + task.temp[m];
        task_table[m] = t;
        if (k < N) {
            int value = task_table[m] + backward[m][order[k]];
            if (value > cmax){
                cmax = value;
            }
        }
    }
    if (k == N){
        return t;
    } 
    return cmax; 
}

vector<int> QNEH(const vector<Task>& data) {
    vector<int> input_order = sort_task_order(data);
    vector<int> order;
    int N_tasks = data.size();
    int M = data[0].temp.size();
    int index = 0;
    vector<vector<int>> forward(M, vector<int>(N_tasks, 0));
    vector<vector<int>> backward(M, vector<int>(N_tasks, 0)); 

    for (int t_index : input_order) {
        int N = order.size();
        int C_max = numeric_limits<int>::max();
        propagate_forward(data, order, forward, index);
        propagate_backward(data, order, backward, index);
        for (int i = 0; i < N+1; i++){
            int c = getCmax(data[t_index], forward, backward, i, order);
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
        if (line.empty()){
            save_data = false;
            if (!current_dataset.empty()) {
                datasets.push_back(current_dataset);
                current_dataset.clear();
            }
            continue;
        }
        if (line.find("data.") != string::npos){
            save_data = true;
            counter = 0;
        } else {
            if (save_data){
                if (counter == 0){
                    counter++;
                    continue;
                } 
                istringstream iss(line);
                int num;
                int sum_times = 0;
                vector<int> task_times;
                while (iss >> num) {
                    sum_times += num;
                    task_times.push_back(num);
                }
                current_dataset.push_back({counter, task_times, sum_times});
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
    chrono::duration<double> total_time = chrono::duration<double>::zero();
    for (int i = data_from; i <= data_to; i++) {
        printf("data.%03d: Cmax: ", i);
        auto start = chrono::high_resolution_clock::now();
        vector<int> result = QNEH(datasets[i]);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end-start;

        total_time += duration;
        int N = datasets[i].size();
        int M = datasets[i][0].temp.size();
        int cmax = 0;
        vector<int> prev(M, 0);
        for (int t_index : result) {
            int t = 0;
            for (int m = 0; m < M; m++) {
                t = max(t, prev[m]) + datasets[i][t_index].temp[m];
                prev[m] = t;
                cmax = t;
            }
        }
        cout << cmax << " ";
        cout << "Czas: " << duration.count() << endl;
    }
    cout << "Czas dzialania programu: " << total_time.count() << " s"<< endl;

    return 0;
}
