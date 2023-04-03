#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include <iomanip>

using namespace std;

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
    stream << "\r\n";
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

        #ifdef _WIN32
                const int IGNORE_CHARS = 1;
        #else
                const int IGNORE_CHARS = 2;
        #endif
        if (infile.is_open()) {
            Alumno tmp;
            while (infile.read((char *) &tmp, sizeof(tmp))) {
                infile.ignore(IGNORE_CHARS);
                result.push_back(tmp);
            }
            infile.close();
        }
        return result;
    }
    void add(const Alumno& record){
        std::ofstream  outfile;
        //refill(record);
        outfile.open(this->filename, std::ios::app);
        outfile.write((char*)&record, sizeof(record));
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

int main(){
    FixedRecord fr("datos1.txt");
    Alumno a;
    std::strcpy(a.codigo, "0111 ");
    std::strcpy(a.nombre, "juaquin    ");
    std::strcpy(a.apellidos, "Remon00                         ");
      std::strcpy(a.carrera, "Computacion          ");
    //fr.add(a);
    auto t = fr.load();
    std::cout << t.size() << std::endl;
    for (auto & e: t){
        /*for (int i =0; i<15; i++){
            std::cout << e.carrera[i];
        }
        std::cout << std::endl;
        */
        std::cout << e;
    }
    std::cout << "---------------------" << std::endl;
    Alumno b = fr.readRecord(5);

    std::cout << b<< std::endl;
//
//
//    std::cout << "-----------------------" << endl;
//    for(size_t i=0; i<t.size();i++){
//        std::cout << t.at(i).codigo << setw(10)
//             << t.at(i).nombre  << setw(10)
//             << t.at(i).apellidos  << setw(10)
//             << t.at(i).carrera << endl;
//    }
    return 0;
}
