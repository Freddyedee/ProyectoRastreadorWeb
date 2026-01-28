#include "../include/AnalizadorGrafo.h"
#include <iostream> 


// Busca el camino más corto desde la página inicial hasta una página cuya URL contenga la palabra clave
std::vector<std::string> AnalizadorGrafo::buscarCaminoConPalabraClave(
    const std::string& paginaInicial,
    const std::string& palabraClave) 
{
    if (grafo.find(paginaInicial) == grafo.end()) {
        return {};  // La página inicial no existe en el grafo
    }

    std::unordered_map<std::string, std::string> padre;
    std::queue<std::string> cola;
    std::unordered_set<std::string> visitadas;

    cola.push(paginaInicial);
    visitadas.insert(paginaInicial);

    bool encontrado = false;
    std::string paginaObjetivo;

    while (!cola.empty()) {
        std::string paginaActual = cola.front();
        cola.pop();

        if (paginaActual.find(palabraClave) != std::string::npos) {
            encontrado = true;
            paginaObjetivo = paginaActual;
            break;
        }

        auto iterador = grafo.find(paginaActual);
        if (iterador == grafo.end()) {
            continue;
        }

        for (const auto& vecino : iterador->second) {
            if (visitadas.find(vecino) == visitadas.end()) {
                visitadas.insert(vecino);
                cola.push(vecino);
                padre[vecino] = paginaActual;
            }
        }
    }

    if (!encontrado) {
        return {};
    }

    std::vector<std::string> camino;
    std::string actual = paginaObjetivo;
    while (actual != paginaInicial) {
        camino.push_back(actual);
        actual = padre[actual];
    }
    camino.push_back(paginaInicial);

    std::reverse(camino.begin(), camino.end());
    return camino;
}

// Calcula algunas estadísticas del sitio web
void AnalizadorGrafo::calcularMetricas(const std::string& paginaInicial)
{
    int totalPaginas = grafo.size();                          // Cuántas páginas tenemos en total
    if (totalPaginas == 0) {
        std::cout << "El grafo está vacío. No hay métricas para calcular.\n";
        return;
    }

    double sumaEnlaces = 0.0;
    int profundidadMaxima = 0;
    std::unordered_set<std::string> subdominiosUnicos;        // Lista de subdominios únicos

    // Contamos enlaces salientes y subdominios
    for (const auto& [url, enlaces] : grafo) {
        sumaEnlaces += enlaces.size();                        // Sumamos todos los enlaces salientes

        std::string dominio = procesador.extraerDominio(url);       
        size_t posicionPunto = dominio.find('.');
        if (posicionPunto != std::string::npos && dominio.substr(0, posicionPunto) != "www") {
            subdominiosUnicos.insert(dominio.substr(0, posicionPunto));
        }
    }

    double gradoPromedio = sumaEnlaces / totalPaginas;        // Promedio de enlaces por página

    // Calculamos la profundidad máxima usando BFS desde la página inicial
    std::queue<std::pair<std::string, int>> cola;
    std::unordered_set<std::string> visitadas;
    cola.push({paginaInicial, 0});
    visitadas.insert(paginaInicial);

    while (!cola.empty()) {
        auto [paginaActual, profundidad] = cola.front();
        cola.pop();

        profundidadMaxima = std::max(profundidadMaxima, profundidad);

        auto iterador = grafo.find(paginaActual);
        if (iterador == grafo.end()) {
            // Página descubierta pero no descargada → saltamos
            continue;
        }

        for (const auto& vecino : grafo.at(paginaActual)) {
            if (visitadas.find(vecino) == visitadas.end()) {
                visitadas.insert(vecino);
                cola.push({vecino, profundidad + 1});
            }
        }
    }

    int diametroAproximado = profundidadMaxima;               // Aproximación simple del diámetro

    // Mostramos los resultados
    std::cout << "Métricas estructurales:\n";
    std::cout << "- Total de páginas únicas: " << totalPaginas << "\n";
    std::cout << "- Profundidad máxima desde la raíz: " << profundidadMaxima << "\n";
    std::cout << "- Grado promedio (enlaces salientes por página): " << gradoPromedio << "\n";
    std::cout << "- Número de subdominios únicos: " << subdominiosUnicos.size() << "\n";
    std::cout << "- Diámetro aproximado (distancia máxima): " << diametroAproximado << "\n";
}

// Opcional: imprimir el grafo para depurar
void AnalizadorGrafo::imprimirCamino(const std::vector<std::string>& camino) const {
    if (camino.empty()) {
        std::cout << "\033[31mNo se encontró camino.\033[0m\n";  // rojo
        return;
    }

    std::cout << "\033[32mCamino encontrado:\033[0m\n";  // verde

    for (size_t i = 0; i < camino.size(); ++i) {
        std::string url = camino[i];
        if (url.find("https://www.") == 0) url = url.substr(12);
        else if (url.find("https://") == 0) url = url.substr(8);

        if (!url.empty() && url.back() == '/') url.pop_back();

        std::cout << "\033[36m" << url << "\033[0m";  // cian
        if (i < camino.size() - 1) {
            std::cout << " \033[33m ->\033[0m ";  // amarillo para flecha
        }
    }
    std::cout << " \033[32m(fin)\033[0m\n";
}