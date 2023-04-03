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
            int header;
            infile.read((char *) &header, sizeof(header));

            Alumno record;
            while(infile.read((char *) &record, sizeof(record))){

                bool eliminado;
                infile.read((char *) &eliminado, sizeof(eliminado));

                if (!eliminado) {
                    result.push_back(record);
                }

                int next_del;
                infile.read((char *) &next_del, sizeof(next_del));
            }
            infile.close();
        }
        return result;
    };
    void add(const Alumno& record){
        std::fstream file;
        file.open(this->filename, std::ios::in | std::ios::out | std::ios::binary);
        int header;

        if(!file) {
            file.close();

            std::ofstream outfile;
            outfile.open(this->filename, std::ios::app | std::ios::binary);
            header = -1;
            outfile.write((char*) &header, sizeof(header));
            outfile.close();

            file.open(this->filename, std::ios::in | std::ios::out | std::ios::binary);
        } else {
            file.read((char *) &header, sizeof(header));
        }

        bool eliminado = false;
        int next_del = 0;

        if (header == -1){
            file.close();
            std::ofstream outfile;
            outfile.open(this->filename, std::ios::app | std::ios::binary);
            outfile.write((char*) &record, sizeof(record));

            outfile.write((char*) &eliminado, sizeof(eliminado));
            outfile.write((char*) &next_del, sizeof(next_del));

            outfile.close();
            return;
        } else {
            file.seekg(sizeof(header) + (header)*(sizeof(record) + sizeof(eliminado) + sizeof(next_del)) + sizeof(record) + sizeof(eliminado), std::ios::beg);

            file.read((char *) &next_del, sizeof(next_del));

            file.seekg(0, std::ios::beg);
            file.write((char *) &next_del, sizeof(next_del));

            file.seekg(sizeof(header) + (header)*(sizeof(record) + sizeof(eliminado) + sizeof(next_del)), std::ios::beg);
            next_del = 0;

            file.write((char *) &record, sizeof(record));
            file.write((char *) &eliminado, sizeof(eliminado));
            file.write((char *) &next_del, sizeof(next_del));
        }

    };

    Alumno readRecord(int pos){
        std::ifstream infile;
        infile.open(this->filename, std::ios::binary);

        Alumno record;
        bool eliminado;

        infile.seekg(sizeof(int) + pos * (sizeof(record) + sizeof(eliminado) + sizeof(int)) , std::ios::beg);
        infile.read((char *) &record, sizeof(record));

        infile.read((char *) &eliminado, sizeof(eliminado));

        infile.close();
        if (!eliminado) {
            return record;
        } else {
            Alumno empty;
            return empty;
        }
    };

    bool del(int pos){
        std::fstream file;
        file.open(this->filename, std::ios::in | std::ios::out | std::ios::binary);

        int header;
        file.read((char *) &header, sizeof(header));
        file.seekg(0, std::ios::beg);
        file.write((char *) &pos, sizeof(pos));

        Alumno record;
        bool eliminado;
        int next_del;

        file.seekg(sizeof(header) + (pos)*(sizeof(record) + sizeof(eliminado) + sizeof(next_del)) + sizeof(record), std::ios::beg);

        file.read((char *) &eliminado, sizeof(eliminado));

        file.read((char *) &next_del, sizeof(next_del));

        if (header == -1) {
            eliminado = true;
            next_del = -1;
        } else {
            if (eliminado) {
                file.close();
                return false;
            }
            eliminado = true;
            next_del = header;
        }

        file.seekg(sizeof(header) + (pos)*(sizeof(record) + sizeof(eliminado) + sizeof(next_del)) + sizeof(record), std::ios::beg);
        file.write((char *) &eliminado, sizeof(eliminado));
        file.write((char *) &next_del, sizeof(next_del));

        file.close();
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
    assignString2CharArray("0010", a.codigo, sizeof(a.codigo));
    assignString2CharArray("juaquin312   ", a.nombre, sizeof(a.nombre));
    assignString2CharArray("Remon00                         ", a.apellidos, sizeof(a.apellidos));
    assignString2CharArray("Computacion          ", a.carrera, sizeof(a.carrera));

    a.ciclo = 1;
    a.mensualidad = 0.99;

    fr.add(a);

    assignString2CharArray("0011", a.codigo, sizeof(a.codigo));
    fr.add(a);

    assignString2CharArray("0012", a.codigo, sizeof(a.codigo));
    fr.add(a);

    assignString2CharArray("0013", a.codigo, sizeof(a.codigo));
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
    Alumno b = fr.readRecord(0);

    std::cout << b.codigo << " ";
    std::cout << b.nombre << " ";
    std::cout << b.apellidos << " ";
    std::cout << b.carrera << " ";
    std::cout << b.ciclo << " ";
    std::cout << b.mensualidad << std::endl;

    std::cout << "---------------------" << std::endl;
    /* fr.del(1);
    fr.del(2);
    fr.del(3);
    t = fr.load();
    for (auto & e: t){
        std::cout << e.codigo << " ";
        std::cout << e.nombre << " ";
        std::cout << e.apellidos << " ";
        std::cout << e.carrera << " ";
        std::cout << e.ciclo << " ";
        std::cout << e.mensualidad << std::endl;
    } */


}