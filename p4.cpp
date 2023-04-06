#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>

struct Matricula
{
    std::string codigo;
    int ciclo;
    float mensualidad;
    std::string observaciones;

    int size_bytes()
    {
        int sz_fields = this->codigo.size() + sizeof(this->ciclo) + sizeof(this->mensualidad) + this->observaciones.size();
        int sz_prefix_fields = 2 * sizeof(int);
        return sz_fields + sz_prefix_fields;
    }

    void desempaquetar(char *buffer, int sz_reg)
    {
        // Codigo
        int sz_codigo;
        std::memcpy(&sz_codigo, buffer, sizeof(sz_codigo));

        buffer += sizeof(sz_codigo);

        char *buffer_codigo = new char[sz_codigo];
        std::memcpy(buffer_codigo, buffer, sz_codigo);
        buffer += sz_codigo;

        this->codigo = std::string(buffer_codigo, sz_codigo);
        delete[] buffer_codigo;

        // Ciclo
        memcpy(&this->ciclo, buffer, sizeof(this->ciclo));
        buffer += sizeof(this->ciclo);

        // Mensualidad
        memcpy(&this->mensualidad, buffer, sizeof(this->mensualidad));
        buffer += sizeof(this->mensualidad);

        // Observaciones
        int sz_observaciones;
        std::memcpy(&sz_observaciones, buffer, sizeof(sz_observaciones));

        buffer += sizeof(sz_observaciones);

        char *buffer_observaciones = new char[sz_observaciones];
        std::memcpy(buffer_observaciones, buffer, sz_observaciones);
        buffer += sz_observaciones;

        this->observaciones = std::string(buffer_observaciones, sz_observaciones);
        delete[] buffer_observaciones;
    }

    char *empaquetar()
    {

        char *buffer = new char[this->size_bytes()];
        char *start_buffer = buffer;

        int sz_codigo = this->codigo.size();

        std::memcpy(buffer, &sz_codigo, sizeof(int));
        buffer += sizeof(int);
        std::memcpy(buffer, this->codigo.c_str(), sz_codigo);
        buffer += sz_codigo;

        std::memcpy(buffer, &this->ciclo, sizeof(int));
        buffer += sizeof(int);

        std::memcpy(buffer, &this->mensualidad, sizeof(float));
        buffer += sizeof(float);

        int sz_observaciones = this->observaciones.size();

        std::memcpy(buffer, &sz_observaciones, sizeof(int));
        buffer += sizeof(int);
        std::memcpy(buffer, this->observaciones.c_str(), sz_observaciones);
        buffer += sz_observaciones;

        return start_buffer;
    }
};

std::ostream & operator<<(std::ostream & stream, Matricula & matricula){
    stream<<matricula.codigo<<" ";
    stream<<matricula.ciclo<<" ";
    stream<<matricula.mensualidad<<" ";
    stream<<matricula.observaciones<<" ";
    return stream;
}

class VariableRecord
{
    std::string filename;
    std::string metadata_filename;

public:
    VariableRecord(const std::string &filename, const std::string &metadata_filename) : filename(filename), metadata_filename(metadata_filename) {}

    std::vector<Matricula> load()
    {
        std::vector<Matricula> records;

        std::fstream metadata;
        metadata.open(this->metadata_filename, std::ios::in | std::ios::binary);

        std::fstream file;
        file.open(this->filename, std::ios::in | std::ios::binary);

        if (metadata.is_open() && file.is_open())
        {
            int pos_reg;
            int sz_reg;
            Matricula record;
            while (metadata.read((char *)&pos_reg, sizeof(pos_reg)))
            {
                metadata.read((char *)&sz_reg, sizeof(sz_reg));

                char *buffer = new char[sz_reg];

                file.seekg(pos_reg, std::ios::beg);
                file.read((char *)buffer, sz_reg);

                record.desempaquetar(buffer, sz_reg);
                records.push_back(record);

                delete[] buffer;
            }
            metadata.close();
            file.close();
        }

        return records;
    };

    void add(Matricula &record)
    {
        std::fstream metadata;
        metadata.open(this->metadata_filename, std::ios::app | std::ios::binary);

        std::fstream file;
        file.open(this->filename, std::ios::app | std::ios::binary);

        if (metadata.is_open() && file.is_open())
        {

            int sz_reg = record.size_bytes();

            int sz_file = file.tellp();

            metadata.write((char *)&sz_file, sizeof(sz_file));
            metadata.write((char *)&sz_reg, sizeof(sz_reg));

            char *buffer = record.empaquetar();

            file.write((char *)buffer, sz_reg);

            delete[] buffer;

            metadata.close();
            file.close();
        }
    };

    Matricula readRecord(int pos)
    {
        std::fstream metadata;
        metadata.open(this->metadata_filename, std::ios::in | std::ios::binary);

        std::fstream file;
        file.open(this->filename, std::ios::in | std::ios::binary);

        int pos_reg;
        int sz_reg;
        Matricula record;

        if (metadata.is_open() && file.is_open())
        {
            metadata.seekg(0, std::ios::end);
            file.seekg(0, std::ios::end);
            if (metadata.tellg() == 0 || file.tellg() == 0 || metadata.tellg() <= pos * (sizeof(pos_reg) + sizeof(sz_reg)) || pos < 0)
            {
                file.close();
                metadata.close();
                return record;
            }
            else
            {
                metadata.seekg(pos * (sizeof(pos_reg) + sizeof(sz_reg)), std::ios::beg);
                metadata.read((char *)&pos_reg, sizeof(pos_reg));
                metadata.read((char *)&sz_reg, sizeof(sz_reg));

                if (file.tellg() <= pos_reg)
                {
                    file.close();
                    metadata.close();
                    return record;
                }

                char *buffer = new char[sz_reg];

                file.seekg(pos_reg, std::ios::beg);
                file.read((char *)buffer, sz_reg);

                record.desempaquetar(buffer, sz_reg);

                delete[] buffer;
            }
            file.close();
            metadata.close();
        }

        return record;
    };
};

void testWrite(VariableRecord vr){
    std::cout<<"----WRITE----"<<std::endl;
    int sz;
    std::cout<<"Ingrese el numero de elementos a agregar: ";
    std::cin>>sz;
    std::cin.ignore();
    while(sz-- > 0){
        Matricula matricula;

        std::cout<<"Ingrese codigo:"; std::getline(std::cin, matricula.codigo);

        std::cout<<"Ingrese ciclo:"; std::cin>>matricula.ciclo;

        std::cout<<"Ingrese mensualidad:"; std::cin>>matricula.mensualidad;
        std::cin.ignore();
        std::cout<<"Ingrese observaciones:"; std::getline(std::cin, matricula.observaciones);

        vr.add(matricula);

        std::cout<<std::endl;
    }
}

void testRead(VariableRecord vr)
{
    auto records = vr.load();
    auto sz_records = records.size();

    std::cout<<"----READ----"<<std::endl;
    std::cout<<"Cantidad de registros: "<<sz_records<<std::endl;
    if (sz_records == 0) {return;}
    std::cout<<"---------------"<<std::endl;
    for (auto &record : records)
    {
        std::cout << record << std::endl;
    }
    std::cout<<"---------------"<<std::endl;
    int pos_record;
    while (true) {
        std::cout<<"Seleccione indice de registro: "; std::cin>>pos_record;
        if (pos_record >= 0 && pos_record < records.size()){break;}
    }
    Matricula record = vr.readRecord(pos_record);
    std::cout << record << std::endl;
}

int main()
{
    VariableRecord vr("datos_matricula.bin", "cabecera.dat");
    testWrite(vr);
    testRead(vr);
    return 0;
}