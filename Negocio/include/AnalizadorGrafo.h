#ifndef ANALIZADORGRAFO_H
#define ANALIZADORGRAFO_H

#include <string>
#include <iostream>
#include <queue>             // Para usar una cola (primero entra primero sale)
#include <unordered_map>     // Para guardar datos como "clave → valor" muy rapido
#include <unordered_set>     // Para guardar cosas unicas sin repetir (como lista sin duplicados)
#include <vector>            // Para guardar listas de cosas (como array que crece solo)
#include <algorithm>         // Para funciones utiles como transformar texto

class AnalizadorGrafo {
private:
    const std::unordered_map<std::string, std::vector<std::string>>& grafo;
    std::string dominioBase;  // Opcional: guardamos solo el string base para validaciones simples

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

    // Función auxiliar privada para extraer dominio (lógica mínima, sin dependencias externas)
    std::string extraerDominioSimple(const std::string& url) const;

public:
    explicit AnalizadorGrafo(
        const std::unordered_map<std::string, std::vector<std::string>>& g,
        const std::string& dominioBase = "")
        : grafo(g), dominioBase(dominioBase) {}

    void calcularMetricas(const std::string& paginaInicial);
};

#endif //ANALIZADORGRAFO_H