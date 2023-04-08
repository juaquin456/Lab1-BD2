#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include <iomanip>

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
//    assignString2CharArray("0010", a.codigo, sizeof(a.codigo));
//    assignString2CharArray("juaquin312   ", a.nombre, sizeof(a.nombre));
//    assignString2CharArray("Remon00                         ", a.apellidos, sizeof(a.apellidos));
//    assignString2CharArray("Computacion          ", a.carrera, sizeof(a.carrera));
//
//    a.ciclo = 1;
//    a.mensualidad = 0.99;
//    fr.add(a);
//
//    assignString2CharArray("0011", a.codigo, sizeof(a.codigo));
//    fr.add(a);
//
//    assignString2CharArray("0012", a.codigo, sizeof(a.codigo));
//    fr.add(a);
//
//    assignString2CharArray("0013", a.codigo, sizeof(a.codigo));
//    fr.add(a);

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
//
    std::cout << "---------------------" << std::endl;
    fr.del(1);
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
    }

}

void test_P2(){
    FixedRecord records("alumnos_test.dat");

    Alumno record1 = {"0001", "Juan", "Perez", "Ingenieria", 2, 500.0f};
    Alumno record2 = {"0002", "Maria", "Gonzalez", "Medicina", 4, 800.0f};
    Alumno record3 = {"0003", "Carlos", "Rojas", "Derecho", 3, 700.0f};
    Alumno record4 = {"0004", "Esteban", "Obregon", "Marketing", 8, 500.0f};
    Alumno record5 = {"0005", "Pepe", "Milla", "Publicidad", 4, 500.0f};
    Alumno record6 = {"0006", "Anna", "Slavyaninova", "Economia", 5, 700.0f};
    Alumno record7 = {"0007", "Alyssa", "Klee", "Educacion", 2, 300.0f};
    Alumno record8= {"0008", "Lila", "Doering", "Filosofia", 7, 300.0f};
    Alumno record9 = {"0009", "Rodrigo", "Grande", "Gestion", 4, 400.0f};
    Alumno record10 = {"0010", "Kevin", "Osorio", "Derecho", 2, 700.0f};
    Alumno record11 = {"0011", "Newton", "Pinedo", "Ingenieria", 7, 500.0f};
    Alumno record12 = {"0012", "Jorge", "Orellana", "Ingenieria", 9, 500.0f};

    ////// TESTING ADD METHOD /////
    records.add(record1);
    records.add(record2);
    records.add(record3);
    records.add(record4);
    records.add(record5);
    records.add(record6);
    records.add(record7);
    records.add(record8);
    records.add(record9);
    records.add(record10);
    records.add(record11);
    records.add(record12);

    ////// TESTING LOAD METHOD /////
    std::vector<Alumno> loaded_records = records.load();

    std::cout << "Registros cargados: " << std::endl;
    for (Alumno record : loaded_records) {
        std::cout << std::setw(5) << record.codigo
                  << std::setw(10) << record.nombre
                  << std::setw(20) << record.apellidos
                  << std::setw(15) << record.carrera
                  << std::setw(5) << record.ciclo
                  << std::setw(8) << std::fixed << std::setprecision(2) << record.mensualidad << std::endl;
    }

    std::cout << std::endl;
    ////// TESTING DEL METHOD /////
    records.del(2);
    records.del(6);
    records.del(9);
    loaded_records = records.load();

    std::cout << "Registros despues de eliminar los registros 2, 6 y 9: " << std::endl;
    for (Alumno record : loaded_records) {
        std::cout << std::setw(5) << record.codigo
                  << std::setw(10) << record.nombre
                  << std::setw(20) << record.apellidos
                  << std::setw(15) << record.carrera
                  << std::setw(5) << record.ciclo
                  << std::setw(8) << std::fixed << std::setprecision(2) << record.mensualidad << std::endl;
    }

    std::cout << std::endl;

    //// TESTING readRecord METHOD ////
    std::cout << "Registro leido de la posicion 8: " << std::endl;
    Alumno b = records.readRecord(8);
    std::cout << std::setw(5) << b.codigo
              << std::setw(10) << b.nombre
              << std::setw(20) << b.apellidos
              << std::setw(15) << b.carrera
              << std::setw(5) << b.ciclo
              << std::setw(8) << std::fixed << std::setprecision(2) << b.mensualidad << std::endl;
}