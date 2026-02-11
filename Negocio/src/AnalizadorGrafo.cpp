#include "../include/AnalizadorGrafo.h"
#include <iostream> 
#include <iomanip>

void AnalizadorGrafo::calcularMetricas(const std::string& paginaInicial)
{
    if (grafo.empty()) {
        std::cout << "El grafo está vacío. No hay métricas para calcular.\n";
        return;
    }

    // 1. Total páginas y grado promedio
    auto [totalPaginas, gradoPromedio] = calcularTotalPaginasYGradoPromedio();

    // 2. Subdominios
    auto subdominiosUnicos = calcularSubdominiosUnicos();

    // 3. Profundidad máxima (BFS)
    int profundidadMaxima = calcularProfundidadMaxima(paginaInicial);

    // Diámetro aproximado 
    int diametroAproximado = profundidadMaxima;

    // 4. Mostrar resultados
    mostrarMetricas(totalPaginas, gradoPromedio, profundidadMaxima,
                    subdominiosUnicos.size(), diametroAproximado);
}

std::pair<int, double> AnalizadorGrafo::calcularTotalPaginasYGradoPromedio() const
{
    int totalPaginas = grafo.size();
    double sumaEnlaces = 0.0;

    for (const auto& [url, enlaces] : grafo) {
        sumaEnlaces += enlaces.size();
    }

    double gradoPromedio = totalPaginas > 0 ? sumaEnlaces / totalPaginas : 0.0;

    return {totalPaginas, gradoPromedio};
}

std::unordered_set<std::string> AnalizadorGrafo::calcularSubdominiosUnicos()
{
    std::unordered_set<std::string> subdominiosUnicos;

    for (const auto& [url, enlaces] : grafo) {
        std::string dominio = extraerDominioSimple(url);
        size_t posPunto = dominio.find('.');
        if (posPunto != std::string::npos) {
            std::string sub = dominio.substr(0, posPunto);
            if (sub != "www" && !sub.empty()) {
                subdominiosUnicos.insert(sub);
            }
        }
    }

    return subdominiosUnicos;
}

int AnalizadorGrafo::calcularProfundidadMaxima(const std::string& paginaInicial) const
{
    if (grafo.find(paginaInicial) == grafo.end()) {
        return 0;  // Página inicial no existe
    }

    int profundidadMaxima = 0;
    std::queue<std::pair<std::string, int>> cola;
    std::unordered_set<std::string> visitadas;

    cola.push({paginaInicial, 0});
    visitadas.insert(paginaInicial);

    while (!cola.empty()) {
        auto [paginaActual, profundidad] = cola.front();
        cola.pop();

        profundidadMaxima = std::max(profundidadMaxima, profundidad);

        auto it = grafo.find(paginaActual);
        if (it == grafo.end()) continue;

        for (const auto& vecino : it->second) {
            if (visitadas.find(vecino) == visitadas.end()) {
                visitadas.insert(vecino);
                cola.push({vecino, profundidad + 1});
            }
        }
    }

    return profundidadMaxima;
}

void AnalizadorGrafo::mostrarMetricas(int totalPaginas, double gradoPromedio,
                                      int profundidadMaxima, size_t numSubdominios,
                                      int diametroAproximado) const
{
    std::cout << "Métricas estructurales:\n";
    std::cout << "----------------------------------------\n";
    std::cout << "- Total de páginas únicas:              " << totalPaginas << "\n";
    std::cout << "- Profundidad máxima desde la raíz:     " << profundidadMaxima << "\n";
    std::cout << "- Grado promedio (enlaces salientes):   " 
              << std::fixed << std::setprecision(4) << gradoPromedio << "\n";
    std::cout << "- Número de subdominios únicos:         " << numSubdominios << "\n";
    std::cout << "- Diámetro aproximado:                  " << diametroAproximado << "\n";
    std::cout << "----------------------------------------\n";
}

std::string AnalizadorGrafo::extraerDominioSimple(const std::string& url) const {
    if (url.empty()) return "";

    size_t inicio = url.find("://");
    if (inicio == std::string::npos) {
        inicio = 0;
    } else {
        inicio += 3;  // Saltamos ://
    }

    size_t fin = url.find('/', inicio);
    if (fin == std::string::npos) {
        fin = url.size();
    }

    std::string dominio = url.substr(inicio, fin - inicio);

    // Quitamos puerto si existe (:8080)
    size_t puerto = dominio.find(':');
    if (puerto != std::string::npos) {
        dominio = dominio.substr(0, puerto);
    }

    // Quitamos "www." al inicio si existe
    if (dominio.size() > 4 && dominio.substr(0, 4) == "www.") {
        dominio = dominio.substr(4);
    }

    // Todo a minúsculas
    std::transform(dominio.begin(), dominio.end(), dominio.begin(), ::tolower);

    return dominio;
}