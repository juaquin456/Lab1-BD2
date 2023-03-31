#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

struct Alumno{
    char codigo [5];
    char nombre [11];
    char apellidos [20];
    char carrera [15];

    int ciclo;
    float mensualidad;
};

std::istream & operator>>(std::istream & stream, Alumno& record){
    stream.read(record.codigo, 5);
    stream.read(record.nombre, 11);
    stream.read(record.apellidos, 20);
    stream.read(record.carrera, 15);
    stream.get();
    return stream;
}

std::ostream & operator<<(std::ostream & stream, Alumno & record){
    stream.write(record.codigo, 5);
    stream.write(record.nombre, 11);
    stream.write(record.apellidos, 20);
    stream.write(record.carrera, 15);
    stream << "\n";
    stream<<flush;
    return stream;
}

class FixedRecord{
    string filename;
public:
    explicit FixedRecord(string filename){
        this->filename = filename;
    };

    vector<Alumno> load() {
        vector<Alumno> alumnos;

        ifstream inFile;
        inFile.open(this->filename, ios::in | ios::binary);
        if (inFile.is_open()) {
            Alumno record;
            while(inFile.read((char*) &record, sizeof(record))){
                alumnos.push_back(record);
            }
            inFile.close();
        }
        return alumnos;
    };

    void add(Alumno record){
        ofstream outFile;
        outFile.open(this->filename, ios::app | ios::binary);
        outFile.write((char*) &record, sizeof(record));
    };

    Alumno readRecord(int pos){
        ifstream inFile;
        Alumno record;
        inFile.open(this->filename, ios::binary);
        inFile.seekg(pos * sizeof(record), ios::beg);
        inFile.read((char *) &record, sizeof(record));
        inFile.close();
        return record;
    };

    bool del(int pos){

    };
};

void test2(){
    FixedRecord fr("datos3.txt");
    Alumno a;
    std::strcpy(a.codigo, "01117878787");
    std::strcpy(a.nombre, "juaquin");
    std::strcpy(a.apellidos, "Remon00");
    std::strcpy(a.carrera, "CS");
    fr.add(a);
    auto t = fr.load();
    std::cout << t.size() << std::endl;
    for (auto & e: t){
        std::cout << e.codigo << " ";
        std::cout << e.nombre << " ";
        std::cout << e.apellidos << " ";
        std::cout << e.carrera << std::endl;
    }
    std::cout << "---------------------" << std::endl;
    Alumno b = fr.readRecord(0);

    std::cout << b.codigo << " ";
    std::cout << b.nombre << " ";
    std::cout << b.apellidos << " ";
    std::cout << b.carrera << std::endl;
}