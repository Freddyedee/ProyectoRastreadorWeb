#ifndef AnalizadorGrafo_H
#define AnalizadorGrafo_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include "../include/ProcesadorEnlaces.h"

class BuscadorClave{
    private:
    const std::unordered_map<std::string, std::vector<std::string>>& grafo;

    public:
        explicit BuscadorClave(const std::unordered_map<std::string, std::vector<std::string>>& g)
            : grafo(g) {}

        std::vector<std::string> buscarCaminoConPalabraClave(
            const std::string& paginaInicial,
            const std::string& palabraClave) const;

        void imprimirCamino(const std::vector<std::string>& camino) const;

    private:
        // 1. Validación inicial
        bool validarPaginaInicial(const std::string& paginaInicial) const;

        // 2. Núcleo del BFS: busca y devuelve el objetivo + mapa de padres
        std::pair<bool, std::string> ejecutarBFSParaClave(
            const std::string& paginaInicial,
            const std::string& palabraClave,
            std::unordered_map<std::string, std::string>& padre) const;

        // 3. Reconstruye el camino a partir del objetivo y el mapa de padres
        std::vector<std::string> reconstruirCamino(
            const std::string& paginaInicial,
            const std::string& paginaObjetivo,
            const std::unordered_map<std::string, std::string>& padre) const;
};

#endif