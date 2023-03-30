#include <string>
#include <vector>
#include <fstream>
#include <iostream>

struct Alumno {
    char codigo[5];
    char nombre[11];
    char apellidos[20];
    char carrera[15];
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
    return stream;
}

class FixedRecord {
    std::string filename;
public:
    FixedRecord(const std::string & filename): filename(filename){}
    std::vector<Alumno> load(){
        std::ifstream infile;
        infile.open(this->filename);
        std::vector<Alumno> result;
        if (infile.is_open()) {
            Alumno tmp;
            while (infile.read((char *) &tmp, sizeof(tmp))) {
                result.push_back(tmp);
            }
            infile.close();
        }
        return result;
    }
    void add(const Alumno& record){
        std::ofstream  outfile;
        outfile.open(this->filename, std::ios::app);
        outfile.write((char*)&record, sizeof(record));
        outfile.close();
    }
    Alumno readRecord(int pos){
        return {};
    }
};

int main(){
    FixedRecord fr("datos1.txt");
    Alumno a;
    a.codigo[0]='1';a.codigo[1]='2';
    a.nombre[0]='a';
    a.apellidos[0]='b';
    a.apellidos[1]='n';
    a.carrera[0]='c';
    fr.add(a);
    auto t = fr.load();
    std::cout << t.size() << std::endl;
    for (auto & e: t){
        std::cout << e.codigo << " ";
        std::cout << e.nombre << " ";
        std::cout << e.apellidos << " ";
        std::cout << e.carrera << std::endl;
    }
    return 0;
}
