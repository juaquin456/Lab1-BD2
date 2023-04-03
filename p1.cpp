#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>

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
    return stream;
}

std::ostream & operator<<(std::ostream & stream, Alumno & record){
    stream.write(record.codigo, 5);
    stream.write(record.nombre, 11);
    stream.write(record.apellidos, 20);
    stream.write(record.carrera, 15);
    stream << std::flush;
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
                infile.ignore(2);
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
        outfile << "\r\n";
        outfile.close();
    }
    Alumno readRecord(int pos){
        std::ifstream infile;
        infile.open(this->filename);
        Alumno tmp;
        infile.seekg(pos*sizeof(tmp)+2*pos);
        infile.read((char *) &tmp, sizeof(tmp));
        infile.close();
        return tmp;
    }
};

void test1() {
    FixedRecord fr("datos1.txt");
    Alumno a;
    std::strcpy(a.codigo, "0118 ");
    std::strcpy(a.nombre, "juaquin         ");
    std::strcpy(a.apellidos, "Remon04             ");
    std::strcpy(a.carrera, "Computacion    ");
    fr.add(a);
    auto t = fr.load();
    std::cout << t.size() << std::endl;
    for (auto &e: t) {
        std::cout << e << std::endl;
    }
    std::cout << "---------------------" << std::endl;
    std::cout << "Record 8: \n";
    Alumno b = fr.readRecord(8);
    std::cout << b << std::endl;
}

int main(){
    test1();
}
