#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>

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
    stream.read(reinterpret_cast<char *>(&record.ciclo), sizeof(record.ciclo));
    stream.read(reinterpret_cast<char *>(&record.mensualidad), sizeof(record.mensualidad));

    stream.get();
    return stream;
}

std::ostream & operator<<(std::ostream & stream, Alumno & record){
    stream.write(record.codigo, 5);
    stream.write(record.nombre, 11);
    stream.write(record.apellidos, 20);
    stream.write(record.carrera, 15);
    stream.write(reinterpret_cast<char *>(&record.ciclo), sizeof(record.ciclo));
    stream.write(reinterpret_cast<char *>(&record.mensualidad), sizeof(record.mensualidad));
    stream << "\r\n";
    stream<<std::flush;
    return stream;
}

class FixedRecord{
    std::string filename;
public:
    FixedRecord(const std::string & filename): filename(filename){}
    std::vector<Alumno> load() {
        std::ifstream infile;
        infile.open(this->filename, std::ios::in | std::ios::binary);
        std::vector<Alumno> result;
        if (infile.is_open()) {
            Alumno tmp;
            while(infile.read((char *) &tmp, sizeof(tmp))){
                result.push_back(tmp);
            }
            infile.close();
        }
        return result;
    };
    void add(const Alumno& record){
        std::ofstream outfile;
        outfile.open(this->filename, std::ios::app | std::ios::binary);
        outfile.write((char*) &record, sizeof(record));
        outfile.close();
    };

    Alumno readRecord(int pos){
        std::ifstream infile;
        infile.open(this->filename, std::ios::binary);
        Alumno tmp;
        infile.seekg(pos * sizeof(tmp), std::ios::beg);
        infile.read((char *) &tmp, sizeof(tmp));
        infile.close();
        return tmp;
    };

    bool del(int pos){
        return true;
    };
};

void assignString2CharArray(std::string temp, char buffer[], int size){
    for(int i=0; i<size; i++)
        buffer[i] = (i < temp.size())? temp[i] : ' ';
    buffer[size-1] = '\0';
}

void test2(){
    FixedRecord fr("datos_alumnos.bin");
    Alumno a = Alumno();
    assignString2CharArray("0001", a.codigo, sizeof(a.codigo));
    assignString2CharArray("juaquin3    ", a.nombre, sizeof(a.nombre));
    assignString2CharArray("Remon00                         ", a.apellidos, sizeof(a.apellidos));
    assignString2CharArray("Computacion          ", a.carrera, sizeof(a.carrera));

    a.ciclo = 1;
    a.mensualidad = 0.99;

    fr.add(a);
    auto t = fr.load();
    std::cout << t.size() << std::endl;
    for (auto & e: t){
        std::cout << e.codigo << " ";
        std::cout << e.nombre << " ";
        std::cout << e.apellidos << " ";
        std::cout << e.carrera << " ";
        std::cout << e.ciclo << " ";
        std::cout << e.mensualidad << std::endl;
    }
    std::cout << "---------------------" << std::endl;
    Alumno b = fr.readRecord(10);

    std::cout << b.codigo << " ";
    std::cout << b.nombre << " ";
    std::cout << b.apellidos << " ";
    std::cout << b.carrera << " ";
    std::cout << b.ciclo << " ";
    std::cout << b.mensualidad << std::endl;
}