#include "../include/GrafoWeb.h"
#include <algorithm> 

void GrafoWeb::agregarNodo(const std::string& url) {
    if (!existeNodo(url)) {
        adyacencias[url] = std::vector<std::string>();
    }
}

void GrafoWeb::agregarArista(const std::string& origen, const std::string& destino) {
    agregarNodo(origen);
    agregarNodo(destino);

    // Solo agregamos si no existe ya esa conexión específica
    auto& lista = adyacencias[origen];
    if (std::find(lista.begin(), lista.end(), destino) == lista.end()) {
        lista.push_back(destino);
    }
}

bool GrafoWeb::existeNodo(const std::string& url) const {
    return adyacencias.find(url) != adyacencias.end();
}

/**
 * @brief Devuelve referencia constante al grafo interno.
 */
const std::unordered_map<std::string, std::vector<std::string>>& 
GrafoWeb::getGrafo() const {
    return adyacencias;  
}
