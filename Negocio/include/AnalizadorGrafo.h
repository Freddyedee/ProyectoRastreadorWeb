#ifndef AnalizadorGrafo_H
#define AnalizadorGrafo_H

#include <string>
#include <iostream>
#include <queue>             // Para usar una cola (primero entra primero sale)
#include <unordered_map>     // Para guardar datos como "clave → valor" muy rapido
#include <unordered_set>     // Para guardar cosas unicas sin repetir (como lista sin duplicados)
#include <vector>            // Para guardar listas de cosas (como array que crece solo)
#include <algorithm>         // Para funciones utiles como transformar texto
#define NOMINMAX  


class AnalizadorGrafo {
private:
    const std::unordered_map<std::string, std::vector<std::string>>& grafo;

public:
    explicit AnalizadorGrafo(const std::unordered_map<std::string, std::vector<std::string>>& g)
        : grafo(g) {}

    // Imprime el camino de forma limpia y legible
    void imprimirCamino(const std::vector<std::string>& camino) const;

    std::vector<std::string> buscarCaminoConPalabraClave(
    const std::string& paginaInicial,
    const std::string& palabraClave);

    void calcularMetricas(const std::string& start);
    // Puedes agregar más: shortestPath entre dos nodos, degree centrality, etc.
};

#endif
