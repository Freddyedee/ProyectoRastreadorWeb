#include "../include/GestorArchivos.h"
#include <fstream>
#include <iostream>

void GestorArchivos::guardarGrafo(const std::unordered_map<std::string, std::vector<std::string>>& grafo, const std::string& archivo) {
    std::ofstream out("grafo.txt");  // Nombre simple, se guarda en la carpeta del .exe
    if (!out.is_open()) {
        std::cerr << "Error: No se pudo crear el archivo grafo.txt (verifica permisos)\n";
        return;
    }

    out << "GRAFO DE ENLACES - Total paginas: " << grafo.size() << "\n\n";
    for (const auto& par : grafo) {
        out << par.first << ":\n";
        for (const auto& enlace : par.second) {
            out << "  -> " << enlace << "\n";
        }
        if (par.second.empty()) {
            out << "  (sin enlaces salientes)\n";
        }
        out << "\n";
    }
    out.close();
    std::cout << "Grafo guardado correctamente en grafo.txt\n";
}

void GestorArchivos::guardarLista(
    const std::vector<std::string>& datos,
    const std::string& nombreArchivo
) {
    std::ofstream archivo(nombreArchivo);

    if (!archivo.is_open()) return;

    for (const auto& item : datos) {
        archivo << item << "\n";
    }

    archivo.close();
}
