#include "../include/BuscadorClave.h"

#include<iostream>

// Busca el camino más corto desde la página inicial hasta una página cuya URL contenga la palabra clave
bool BuscadorClave::validarPaginaInicial(const std::string& paginaInicial) const {
    if (grafo.find(paginaInicial) == grafo.end()) {
        return false;  // No existe la página inicial
    }
    return true;
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

        // ¡Encontramos la página objetivo!
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