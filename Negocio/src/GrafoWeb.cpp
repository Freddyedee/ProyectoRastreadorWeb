#include "../include/GrafoWeb.h"

void GrafoWeb::agregarNodo(const std::string& url) {
    if (!existeNodo(url)) {
        adyacencias[url] = std::vector<std::string>();
    }
}

void GrafoWeb::agregarArista(const std::string& origen, const std::string& destino) {
    agregarNodo(origen);
    agregarNodo(destino);
    adyacencias[origen].push_back(destino);
}

bool GrafoWeb::existeNodo(const std::string& url) const {
    return adyacencias.find(url) != adyacencias.end();
}

/**
 * @brief  Devuelve el grafo de enlaces generado por el rastreo.
 * 
 * @return const std::unordered_map<std::string, std::vector<std::string>>& Grafo de enlaces.
 */

const std::unordered_map<std::string, std::vector<std::string>>&
GrafoWeb::obtenerGrafo() const {
    return adyacencias;
}

void GrafoWeb::limpiar() {
    adyacencias.clear();
}
