#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Alumno{
public:
    char codigo [5];
    char nombre [11];
    char apellidos [20];
    char carrera [15];

    int ciclo;
    float mensualidad;
};

class FixedRecord{
private:
    string filename;
public:
    FixedRecord(string filename){
        this->filename = filename;
    };

    vector<Alumno> load() {
        vector<Alumno> alumnos;

        ifstream inFile;
        char* record;
        inFile.open(this->filename, ios::in | ios::binary);
        inFile.read((char*) &record, sizeof(record));

        cout<<record<<endl;

        return alumnos;
    }

    /*void add(Alumno record){

    };
    Alumno readRecord(int pos){

    };
    bool del(int pos){

    };*/
};

void test2(){

}