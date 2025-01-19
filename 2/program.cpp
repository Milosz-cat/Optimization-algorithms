#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

struct Task {
    int czas;       // czas trwania
    int waga;       // waga zadania
    int deadline;   // pożądany termin zakończenia
};

int kara_zadania(int t, int d) {
    return max(0, t - d); // Funkcja kary jako maksimum zera i różnicy między faktycznym czasem zakończenia a pożądanym terminem zakończenia
}

int main() {
    Task tablica[100];
    ifstream wczytaj("witi.data.txt");
    if (!wczytaj) {
        cerr << "Nie można otworzyć pliku!" << endl;
        return 1;
    }

    int n;
    string s;
    string s1[11] = {"data.10:", "data.11:", "data.12:", "data.13:", "data.14:", "data.15:", "data.16:", "data.17:", "data.18:", "data.19:", "data.20:"};
    for (int iter = 0; iter < 11; iter++) {
        while (s != s1[iter]) {
            wczytaj >> s;
        }
        cout << "Dane " << s << endl;
        wczytaj >> n;
        for (int i = 0; i < n; i++) {
            wczytaj >> tablica[i].czas >> tablica[i].waga >> tablica[i].deadline;
        }

        int N = 1 << n;
        int* F = new int[N];
        int* poprzednik = new int[N]; // tablica do przechowywania informacji o poprzednikach
        F[0] = 0;
        for (int set = 1; set < N; set++) {
            int c = 0;
            for (int i = 0, b = 1; i < n; i++, b *= 2) {
                if (set & b) {
                    c += tablica[i].czas;
                }
            }
            F[set] = INT_MAX;
            for (int k = 0, b = 1; k < n; k++, b *= 2) {
                if (set & b) {
                    int nowa_wartosc = F[set - b] + tablica[k].waga * max(c - tablica[k].deadline, 0);
                    if (nowa_wartosc < F[set]) {
                        F[set] = nowa_wartosc;
                        poprzednik[set] = k; // aktualizacja poprzednika
                    }
                }
            }
        }

        cout << "Optymalne rozwiazanie: " << F[N - 1] << endl;

        // Wypisanie optymalnej kolejności zadań
        cout << "Optymalna kolejnosc zadan: ";
        int aktualny_set = N - 1;
        vector<int> kolejnosc;
        while (aktualny_set > 0) {
            int indeks_zadania = poprzednik[aktualny_set];
            kolejnosc.push_back(indeks_zadania + 1);
            aktualny_set ^= (1 << indeks_zadania);
        }
        reverse(kolejnosc.begin(), kolejnosc.end());
        for (int zadanie : kolejnosc) {
            cout << zadanie << " ";
        }
        cout << endl;

        delete[] F;
        delete[] poprzednik;
    }
    wczytaj.close();
    cout << endl << "Koniec dzialania programu." << endl;
    cin.get();
    return 0;
}
