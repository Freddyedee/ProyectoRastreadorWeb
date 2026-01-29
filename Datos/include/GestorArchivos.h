#ifndef GESTORARCHIVOS_H
#define GESTORARCHIVOS_H

#include <string>
#include <vector>
#include <unordered_map>

class GestorArchivos {

public:

  static void guardarGrafo(const std::unordered_map<std::string, std::vector<std::string>>& grafo, const std::string& archivo);


    // Guarda una lista de URLs (resultados, caminos, etc.)
    void guardarLista(
        const std::vector<std::string>& datos,
        const std::string& nombreArchivo
    );

};

#endif
