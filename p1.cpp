#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include <iomanip>

#ifdef _WIN32
    const int IGNORE_CHARS = 1;
    std::string newline{"\n"};
#else
    const int IGNORE_CHARS = 2;
    std::string newline{"\r\n"};
#endif

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
    stream.ignore(IGNORE_CHARS);
//    stream.get();
    return stream;
}

std::ostream & operator<<(std::ostream & stream, Alumno & record){
    stream.write(record.codigo, 5);
    stream.write(record.nombre, 11);
    stream.write(record.apellidos, 20);
    stream.write(record.carrera, 15);
//    stream << "\r\n";
    stream << std::endl;
    stream << std::flush;
    return stream;
}

void assignString2CharArray(std::string temp, char buffer[], int size){
    for(int i=0; i<size; i++)
        buffer[i] = (i < temp.size())? temp[i] : ' ';
    buffer[size-1] = ' ';
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
//                infile.get();
//                infile.get();
                infile.ignore(IGNORE_CHARS);
                result.push_back(tmp);
            }
            infile.close();
        }
        return result;
    }
    void add(const Alumno& record){
        std::ofstream  outfile;
        outfile.open(this->filename, std::ios::app);

        if (outfile.is_open()) {
            outfile << newline;
            outfile.write((char *) &record, sizeof(record));
//            outfile << std::endl;
//            outfile << "\r\n";
            outfile.close();
        }
    }
    Alumno readRecord(int pos){
        if (pos < 0) throw std::invalid_argument("La posicion enviada debe no debe ser negativa");
        std::ifstream infile;
        infile.open(this->filename);
        if (infile.is_open()){
            Alumno tmp;
            infile.seekg(pos * sizeof(tmp) + 2*pos, ios::beg);
            infile.read((char *) &tmp, sizeof(tmp));
            infile.close();
            return tmp;
        }
        return {};
    }
};

void record_prev_add(Alumno& record){
    assignString2CharArray(record.codigo, record.codigo, sizeof(record.codigo));
    assignString2CharArray(record.nombre, record.nombre, sizeof(record.nombre));
    assignString2CharArray(record.apellidos, record.apellidos, sizeof(record.apellidos));
    assignString2CharArray(record.carrera, record.carrera, sizeof(record.carrera));
}

void test1() {
    FixedRecord fr("datos1.txt");

    ///// LECTURA DE REGISTROS DEL ARCHIVO ////
    std::cout << "Registros leidos: " << std::endl;
    auto t = fr.load();
    std::cout << t.size() << std::endl;
    for (auto &e: t) {
        std::cout << e;
    }

    //// TESTING ADD METHOD ////
    Alumno c{"0008","Juaquin","Remon Flores","Computacion"};
    Alumno d{"0009","Nicolas","Castanieda","Computacion"};
    Alumno e{"0010","Luis","Torres Osorio","Computacion"};

    record_prev_add(c);
    record_prev_add(d);
    record_prev_add(e);

    fr.add(c);
    fr.add(d);
    fr.add(e);
    std::cout << endl;

    std::cout << string(30,'-') << endl; // Separador de linea
    std::cout << "Registros luego de aniadir nuevo registro: " << std::endl;
    t = fr.load();
    std::cout << t.size() << std::endl;
    for (auto &r: t) {
        std::cout << r;
    }
    std::cout << std::endl;
    std::cout << string(30,'-') << std::endl; // Separador de linea
    std::cout << "Record 0: " << std::endl;
    Alumno b = fr.readRecord(0);
    std::cout << b << std::endl;

}



int main(){
    test1();
}