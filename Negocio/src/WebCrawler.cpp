#include "../include/WebCrawler.h" // Incluye la definición de la clase WebCrawler
#include "../include/ComunicacionHTTP.h" // Incluye la definición de la clase WebCrawler
#include "../include/ProcesadorEnlaces.h" // Incluye la definición de la clase WebCrawler



#include <iostream>   // Entrada / salida estándar
#include <algorithm>


//el proposito de este metodo es implementar un algoritmo de busqueda en anchura (BFS)
//tiene como objetivo explorar paginas web comenzando desde una URL inicial,
// limitar la exploracion con profundidad
//limitar la exploracion por cantidad de paginas
//construir un grafo dirigido de enlaces entre paginas web dentro del mismo dominio

/**
 * @brief Realiza un rastreo web comenzando desde una URL inicial.
 * 
 * Este método implementa el algoritmo de busqueda en anchura (BFS) para explorar
 * paginas web a partir dl mismo dominio. limita el rastreo por profundidad y 
 * numero de paginas visitadas. Construye un grafo dirigido de enlaces entre paginas web.
 * 
 * @param urlInicial URL desde donde comienza el rastreo.
 * @param maxProfundidad  Profundidad máxima de rastreo.
 * @param maxPaginas  Número máximo de páginas a rastrear.
 */
void WebCrawler::rastrear(const std::string& urlInicial, int maxProfundidad, int maxPaginas) {
    // Validación inicial: URL inicial debe ser válida y no vacía
    if (urlInicial.empty() || urlInicial.find("http") != 0) {
        throw std::invalid_argument("URL inicial inválida. Debe empezar con http/https.");
    }

    // 1. Preparación inicial
    ProcesadorEnlaces procesador(urlInicial);
    ComunicacionHTTP comunicacion;

    this->dominio = procesador.extraerDominio(urlInicial);
    if (this->dominio.find("www.") == 0) {
        this->dominio = this->dominio.substr(4);
    }

    // 2. Limpiamos el grafo
    this->grafo.clear();

    // 3. Estructuras para BFS (usaremos URLs normalizadas para evitar duplicados)
    std::queue<std::pair<std::string, int>> paginasPendientes;  // {url_norm, profundidad}
    std::unordered_set<std::string> paginasVisitadas;           // Almacena URLs normalizadas

    // 4. Normalizamos y agregamos nodo inicial
    std::string inicialNorm = procesador.normalizarURL(urlInicial, urlInicial);
    if (inicialNorm.empty()) {
        std::cerr << "URL inicial no pudo normalizarse.\n";
        return;
    }

    paginasPendientes.push({inicialNorm, 0});
    paginasVisitadas.insert(inicialNorm);
    this->grafo.agregarNodo(inicialNorm);

    int paginasProcesadas = 1;

    // 5. BFS principal
    while (!paginasPendientes.empty() && paginasProcesadas < maxPaginas) {
        auto [urlActual, profundidadActual] = paginasPendientes.front();
        paginasPendientes.pop();

        if (profundidadActual >= maxProfundidad) {
            continue;
        }

        // Descargamos la página con manejo de errores
        std::string html;
        try {
            html = comunicacion.descargarPagina(urlActual);
        } catch (const std::exception& e) {
            std::cerr << "Error descargando " << urlActual << ": " << e.what() << "\n";
            continue;
        }

        if (html.empty()) {
            continue;
        }

        // Extraemos enlaces
        std::vector<std::string> enlaces = procesador.extraerEnlaces(html, urlActual);

        for (const std::string& enlace : enlaces) {
            // Normalizamos primero
            std::string enlaceNorm = procesador.normalizarURL(enlace, urlActual);
            if (enlaceNorm.empty()) continue;

            // FILTROS: descartamos lo más rápido posible
            if (enlaceNorm.find('#') == 0) continue;  // anclas

            // Plantillas no resueltas (placeholders de CMS)
            if (enlaceNorm.find("${") != std::string::npos || 
                enlaceNorm.find("{{") != std::string::npos) continue;

            // Enlaces no válidos (javascript:, mailto:, tel:, etc.)
            if (!procesador.esEnlaceValido(enlaceNorm)) continue;

            // Recursos estáticos: PDFs, css, js, imágenes, etc.
            if (procesador.esRecursoEstatico(enlaceNorm)) continue;

            // Solo enlaces del mismo dominio
            if (!procesador.perteneceAlDominio(enlaceNorm)) continue;

            // Si pasó todos los filtros → agregamos si no visitado
            if (paginasVisitadas.find(enlaceNorm) == paginasVisitadas.end()) {
                paginasVisitadas.insert(enlaceNorm);
                paginasPendientes.push({enlaceNorm, profundidadActual + 1});

                // Agregamos al grafo SOLO después de todos los filtros
                this->grafo.agregarNodo(enlaceNorm);
                this->grafo.agregarArista(urlActual, enlaceNorm);

                paginasProcesadas++;

                if (paginasProcesadas >= maxPaginas) {
                    break;
                }
            }
        }

        if (paginasProcesadas >= maxPaginas) {
            break;
        }
    }

    // Mensaje final con métricas básicas
    std::cout << "Rastreo finalizado.\n"
              << "  - Páginas procesadas: " << paginasProcesadas << "\n"
              << "  - Nodos únicos en grafo: " << this->grafo.size() << "\n"
              << "  - Dominio base: " << this->dominio << "\n";
}