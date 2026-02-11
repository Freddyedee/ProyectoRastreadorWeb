#include "../include/BuscadorClave.h"
#include <algorithm> 


#include<iostream>

// Busca el camino más corto desde la página inicial hasta una página cuya URL contenga la palabra clave
bool BuscadorClave::validarPaginaInicial(const std::string& paginaInicial) const {
    if (grafo.find(paginaInicial) == grafo.end()) {
        return false;  // No existe la página inicial
    }
    return true;
}

void BuscadorClave::imprimirCamino(const std::vector<std::string>& camino) const {
    if (camino.empty()) {
        std::cout << "\033[31mNo se encontró camino.\033[0m\n";  // Rojo
        return;
    }

    // Título en verde
    std::cout << "\033[32mCamino encontrado:\033[0m\n";

    for (size_t i = 0; i < camino.size(); ++i) {
        std::string url = camino[i];

        // Limpieza de la URL para que sea más legible
        if (url.find("https://www.") == 0) {
            url = url.substr(12);  // Quita https://www.
        } else if (url.find("https://") == 0) {
            url = url.substr(8);   // Quita https://
        } else if (url.find("http://") == 0) {
            url = url.substr(7);   // Quita http:// (poco común pero posible)
        }

        // Quitar barra final si existe
        if (!url.empty() && url.back() == '/') {
            url.pop_back();
        }

        // Imprimir URL en cian
        std::cout << "\033[36m" << url << "\033[0m";

        // Flecha en amarillo (solo entre elementos)
        if (i < camino.size() - 1) {
            std::cout << " \033[33m ->\033[0m ";
        }
    }

    // Fin en verde
    std::cout << " \033[32m(fin)\033[0m\n";
}

std::pair<bool, std::string> BuscadorClave::ejecutarBFSParaClave(
    const std::string& paginaInicial,
    const std::string& palabraClave,
    std::unordered_map<std::string, std::string>& padre) const
{
    std::queue<std::string> cola;
    std::unordered_set<std::string> visitadas;

    cola.push(paginaInicial);
    visitadas.insert(paginaInicial);

    while (!cola.empty()) {
        std::string paginaActual = cola.front();
        cola.pop();

        if (paginaActual.find(palabraClave) != std::string::npos) {
            return {true, paginaActual};
        }

        auto it = grafo.find(paginaActual);
        if (it == grafo.end()) {
            continue;
        }

        for (const auto& vecino : it->second) {
            if (visitadas.find(vecino) == visitadas.end()) {
                visitadas.insert(vecino);
                cola.push(vecino);
                padre[vecino] = paginaActual;
            }
        }
    }

    return {false, ""};  // No encontrado
}

std::vector<std::string> BuscadorClave::reconstruirCamino(
    const std::string& paginaInicial,
    const std::string& paginaObjetivo,
    const std::unordered_map<std::string, std::string>& padre) const
{
    std::vector<std::string> camino;
    std::string actual = paginaObjetivo;

    while (actual != paginaInicial) {
        camino.push_back(actual);
        auto it = padre.find(actual);
        if (it == padre.end()) {
            // Error inesperado (no debería pasar si BFS fue correcto)
            return {};
        }
        actual = it->second;
    }

    camino.push_back(paginaInicial);
    std::reverse(camino.begin(), camino.end());

    return camino;
}

std::vector<std::string> BuscadorClave::buscarCaminoConPalabraClave(
    const std::string& paginaInicial,
    const std::string& palabraClave) const
{
    // 1. Validación inicial
    if (!validarPaginaInicial(paginaInicial)) {
        return {};
    }

    // 2. Preparación y ejecución del BFS
    std::unordered_map<std::string, std::string> padre;
    auto [encontrado, paginaObjetivo] = ejecutarBFSParaClave(paginaInicial, palabraClave, padre);

    if (!encontrado) {
        return {};
    }

    // 3. Reconstrucción del camino
    return reconstruirCamino(paginaInicial, paginaObjetivo, padre);
}