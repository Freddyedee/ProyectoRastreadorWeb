#ifndef AnalizadorGrafo_H
#define AnalizadorGrafo_H

#include <string>
#include <iostream>
#include <queue>             // Para usar una cola (primero entra primero sale)
#include <unordered_map>     // Para guardar datos como "clave → valor" muy rapido
#include <unordered_set>     // Para guardar cosas unicas sin repetir (como lista sin duplicados)
#include <vector>            // Para guardar listas de cosas (como array que crece solo)
#include <algorithm>         // Para funciones utiles como transformar texto
#include "../include/ProcesadorEnlaces.h"
#define NOMINMAX  


class AnalizadorGrafo {
private:
    const std::unordered_map<std::string, std::vector<std::string>>& grafo;
    ProcesadorEnlaces procesador;

    // Calcula total de páginas y grado promedio
    std::pair<int, double> calcularTotalPaginasYGradoPromedio() const;

    // Calcula y devuelve el conjunto de subdominios únicos
    std::unordered_set<std::string> calcularSubdominiosUnicos();

    // Calcula la profundidad máxima usando BFS
    int calcularProfundidadMaxima(const std::string& paginaInicial) const;

    // Muestra todas las métricas formateadas
    void mostrarMetricas(int totalPaginas, double gradoPromedio,
                         int profundidadMaxima, size_t numSubdominios,
                         int diametroAproximado) const;

public:
    explicit AnalizadorGrafo(
        const std::unordered_map<std::string, std::vector<std::string>>& g,
        const std::string& dominioBase = "")
        : grafo(g), procesador(dominioBase) {}


    void calcularMetricas(const std::string& paginaInicial);
};

#endif
