#ifndef GESTOR_ARCHIVOS_H
#define GESTOR_ARCHIVOS_H

#include <string>

#include <fstream>
#include <string>
#include <iostream>
#include <vector>

class GestorArchivos {
    private: 
        std::string nombreArchivo;

    public:
        GestorArchivos(std::string nombre);
         void limpiarArchivo();
         void guardarEnlace(std::string urlOrigen, std::string urlDestino); 

    };  




#endif // GESTOR_ARCHIVOS_H