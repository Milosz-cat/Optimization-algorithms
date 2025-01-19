#include <iostream>
#include <fstream>
using namespace std;

struct task
{
    int id;
    int r;
    int p;
    int q;
};

int duration(task array[], int size)
{
    int m = 0, c = 0;
    for (int i = 0; i < size; i++)
    {
        m = max(m, array[i].r) + array[i].p;
        c = max(c, m + array[i].q);
    }
    return c;
}

void swap(task arr[], int a, int b)
{
    task temp = arr[a];
    arr[a] = arr[b];
    arr[b] = temp;
}

void sorting(task array[], int size)
{
    int length = duration(array, size);
    int newLength;
    for (int k = 0; k < 10; k++)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                swap(array, i, j);
                newLength = duration(array, size);
                if (newLength > length)
                {
                    swap(array, i, j);
                }
                else
                {
                    length = newLength;
                }
            }
        }
    }
}

int main() {
    char character;
    string str;
    int total = 0;
    int setDuration;
    task tasks[100];
    ifstream read("data.txt");
    string target = "data.4";
    while (str != target) {
        read >> str;
    }
    int n;
    read >> n;
    for (int i = 0; i < n; i++) {
        tasks[i].id = i + 1;
        read >> tasks[i].r;
        read >> tasks[i].p;
        read >> tasks[i].q;
    }
    sorting(tasks, n);
    setDuration = duration(tasks, n);
    total += setDuration;

    cout << "Sum: " << total << endl;
    read.close();
    cin >> character;
}
