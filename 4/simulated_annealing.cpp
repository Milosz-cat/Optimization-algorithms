#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
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

vector<int> swap_random(const vector<int>& order) {
    vector<int> new_order = order;
    int index1 = rand() % order.size();
    int index2;
    do {
        index2 = rand() % order.size();
    } while (index1 == index2);
    swap(new_order[index1], new_order[index2]);
    return new_order;
}

vector<int> simulated_annealing(const vector<Task>& data, int iterations, double start_temp, double cooling_rate) {
    vector<int> order(data.size());
    ofstream result_file("results.csv", ofstream::out); // plik wynikowy

    for (size_t i = 0; i < order.size(); i++) {
        order[i] = i;  // Inicjalizuj wartości
    }

    int current_cmax = calculate_cmax(data, order);

    srand(static_cast<unsigned int>(time(nullptr)));  // Inicjalizacja generatora losowego

    double temp = start_temp;

    for (int i = 0; i < iterations; i++) {
        vector<int> new_order = swap_random(order);

        int new_cmax = calculate_cmax(data, new_order);
        int delta_cmax = new_cmax - current_cmax;

        if (delta_cmax < 0) {
            order = new_order;
            current_cmax = new_cmax;
        } else {
            double probability = exp(-delta_cmax / temp);
            if (static_cast<double>(rand()) / RAND_MAX < probability) {
                order = new_order;
                current_cmax = new_cmax;
            }
        }
        if (i % 100 == 0) {
            result_file << i << ", " << current_cmax << endl;
        }

        temp *= cooling_rate;
    }
    result_file.close();
    return order;
}

pair<double, double> calculate_temperatures(int D_max, int D_min) {
    if (D_max <= 0 || D_min <= 0) {
        throw invalid_argument("D_max and D_min must be greater than 0.");
    }
    double t_high = -D_max / log(0.9);
    double t_low = -D_min / log(0.1);
    return {t_high, t_low};
}

double calculate_cooling_rate(double t_high, double t_low, int iterations) {
    if (t_high <= 0 || t_low <= 0) {
        throw invalid_argument("t_high and t_low must be greater than 0.");
    }
    return pow(t_low / t_high, 1.0 / iterations);
}

pair<int, int> find_extreme_deltas(const vector<Task>& data, int changes) {
    vector<int> order(data.size());
    for (size_t i = 0; i < order.size(); i++) order[i] = i;

    int current_cmax = calculate_cmax(data, order);
    int D_max = INT_MIN;
    int D_min = INT_MAX;

    for (int i = 0; i < changes; i++) {
        vector<int> new_order = swap_random(order);
        int new_cmax = calculate_cmax(data, new_order);
        int delta = abs(new_cmax - current_cmax);

        if (delta >= D_max) D_max = delta;
        if (delta <= D_min) D_min = delta;

        current_cmax = new_cmax; // Aktualizacja bieżącej wartości Cmax
    }

    
    if (D_min == 0) D_min = 1;
    return {D_max, D_min};
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

    cout << "Symulowane Wywazanie - Wyniki" << endl;

    chrono::duration<double> total_time = chrono::duration<double>::zero();

    int iterations = 100000;  // Liczba iteracji

    for (int i = data_from; i <= data_to; i++) {
        auto extremes = find_extreme_deltas(datasets[i], 1000);

        auto temperatures = calculate_temperatures(extremes.first, extremes.second);
        
        double start_temp = temperatures.first;
        double cooling_rate = calculate_cooling_rate(start_temp, temperatures.second, iterations);

        auto start = chrono::high_resolution_clock::now();
        vector<int> result = simulated_annealing(datasets[i], iterations, start_temp, cooling_rate);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        total_time += duration;


        cout << "data." << i << ": Cmax: " << calculate_cmax(datasets[i], result) << " ";
        cout << "| Czas: " << duration.count() << " s ";
        cout << "|Start_temp: " << start_temp << " |Cooling_rate: " << cooling_rate << endl;
            
    }

    cout << "Czas działania programu: " << total_time.count() << " s" << endl;

    return 0;
}
