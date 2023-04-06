//
// Created by juaquin on 03/04/23.
//
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

struct Alumno{
    std::string Nombre;
    std::string Apellidos;
    std::string Carrera;
    float mensualidad;
};

class VariableRecord {
    std::string filename;
    char field_delim = '|';
    char record_delim = '\n';

    void _build_from_line(std::stringstream & lineStream, Alumno& record){
        std::getline(lineStream, record.Nombre, this->field_delim);
        std::getline(lineStream, record.Apellidos, this->field_delim);
        std::getline(lineStream, record.Carrera, this->field_delim);
        std::string float_temp;
        std::getline(lineStream, float_temp, this->field_delim);
        record.mensualidad = std::stof(float_temp);
    }

    void _read_first_line(std::ifstream & file){
        std::string tmp;
        getline(file, tmp);
    }

public:
    VariableRecord(const std::string & name): filename(name){}
    std::vector<Alumno> load(){
        std::ifstream infile;
        infile.open(this->filename);
        std::vector<Alumno> result;

        if (infile.is_open()) {
            this->_read_first_line(infile);
            Alumno tmp;
            std::string line;
            while (std::getline(infile,line,this->record_delim)) {
                Alumno temp;
                std::stringstream lineStream(line);
                this->_build_from_line(lineStream, temp);
                result.push_back(temp);
            }
            infile.close();
        }
        return result;
    }

    void add(Alumno & record) {
        std::ofstream  outfile;
        outfile.open(this->filename, std::ios::app);
        if (outfile.is_open()) {
            outfile << record.Nombre << this->field_delim;
            outfile << record.Apellidos << this->field_delim;
            outfile << record.Carrera << this->field_delim;
            outfile << std::to_string(record.mensualidad) << this->record_delim;
            outfile.close();
        }
    }

    Alumno readRecord(int pos) {
        if (pos < 0) throw std::invalid_argument("La posicion enviada debe no debe ser negativa");
        std::ifstream infile;
        infile.open(this->filename);
        if (infile.is_open()) {
            this->_read_first_line(infile);
            Alumno tmp;
            std::string line;
            for (int i = 0; i <= pos; i++) {
                getline(infile, line, this->record_delim);
            }
            infile.close();
            std::stringstream lineStream(line);
            this->_build_from_line(lineStream, tmp);
            return tmp;
        }
        return {};
    }
};

void test1() {
    VariableRecord vr("datos2.txt");
    vr.readRecord(-1);
    std::cout << vr.load().size() << std::endl;
    for (auto& e: vr.load()) {
        std::cout << e.Nombre << "\t" << e.Apellidos << "\t" << e.Carrera << "\t" << e.mensualidad << std::endl;
    }
    std::cout << typeid(vr.readRecord(1).mensualidad).name() << std::endl;
    Alumno b;
    b.Nombre = "Juaquin";
    b.Apellidos = "Remon Flores";
    b.Carrera = "Computacion";
    b.mensualidad = 1000.99;
    vr.add(b);
    for (auto& e: vr.load()) {
        std::cout << e.Nombre << "\t" << e.Apellidos << "\t" << e.Carrera << "\t" << e.mensualidad << std::endl;
    }
    std::cout << vr.readRecord(5).Nombre << std::endl;
}

int main() {
    test1();
}