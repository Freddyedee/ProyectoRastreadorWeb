#include "GestorArchivos.h"

GestorArchivos::GestorArchivos(std::string nombre): nombreArchivo(nombre) {}

//Delete the previous content of the file on started a new analysis
void GestorArchivos::limpiarArchivo() {
    std::ofstream archivo(this->nombreArchivo, std::ios::trunc);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo para limpiar: " << this->nombreArchivo << std::endl;
        return;
    }
    archivo.close();
}

//save the relation father -> son in the text file 
//the reuquest "save in the file all links in the page"

void GestorArchivos::guardarEnlace(std::string urlOrigen, std::string urlDestino){
    std::ofstream archivo(this->nombreArchivo, std::ios::app);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo para guardar el enlace: " << this->nombreArchivo << std::endl;
        return;
    }
    archivo << urlOrigen << " -> " << urlDestino << std::endl;
    archivo.close();
}


