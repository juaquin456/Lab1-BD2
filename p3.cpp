//
// Created by juaquin on 03/04/23.
//
#include <string>
#include <vector>

struct Alumno{
    std::string Nombre;
    std::string Apellidos;
    std::string Carrera;
    float mensualidad;
};

class VariableRecord {
    std::string filename;
public:
    VariableRecord(const std::string & name): filename(name){}
    std::vector<Alumno> load(){

    }

    void add(Alumno record) {

    }

    Alumno readRecord(int pos) {

    }
};