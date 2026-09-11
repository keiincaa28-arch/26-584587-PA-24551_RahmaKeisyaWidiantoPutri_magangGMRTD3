#include <iostream>
#include <string>
using namespace std;

class KomponenROV {
protected:
    string nama;
    string status;
public:
    KomponenROV(string nama, string status) : nama(nama), status(status) {}
    virtual void info() {
        cout << "Nama: " << nama << ", Status: " << status << endl;
    }
    virtual ~KomponenROV() {}
};

class Thruster : public KomponenROV {
private:
    int power;
public:
    Thruster(string nama, string status, int power)
        : KomponenROV(nama, status), power(power) {}

    void info() override {
        cout << "[Thruster] Nama: " << nama
             << ", Status: " << status
             << ", Power: " << power << endl;
    }
};

class Sensor : public KomponenROV {
private:
    int nilai;
public:
    Sensor(string nama, string status, int nilai)
        : KomponenROV(nama, status), nilai(nilai) {}

    void info() override {
        cout << "[Sensor] Nama: " << nama
             << ", Status: " << status
             << ", Nilai: " << nilai << endl;
    }
};

int main() {
    int n;
    cin >> n;

    KomponenROV** komponenList = new KomponenROV*[n];

    for (int i = 0; i < n; i++) {
        string jenis, nama, status;
        int angka;
        cin >> jenis >> nama >> status >> angka;

        if (jenis == "Thruster") {
            komponenList[i] = new Thruster(nama, status, angka);
        } else if (jenis == "Sensor") {
            komponenList[i] = new Sensor(nama, status, angka);
        } else {
            komponenList[i] = nullptr;
        }
    }

    for (int i = 0; i < n; i++) {
        if (komponenList[i]) komponenList[i]->info();
    }

    for (int i = 0; i < n; i++) delete komponenList[i];
    delete[] komponenList;

    return 0;
}