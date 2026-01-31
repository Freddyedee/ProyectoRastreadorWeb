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
void WebCrawler::rastrear(const std::string& urlInicial, 
                          int maxProfundidad, 
                          int maxPaginas, 
                          GrafoWeb& grafoDestino) 
{
    // PASO 1: Inicializa el dominio base
    ProcesadorEnlaces procesador(urlInicial);
    ComunicacionHTTP comunicacion;

    this->dominio = procesador.extraerDominio(urlInicial);  // ← asignamos a miembro

    // Quitamos "www." si existe (para normalizar)
    if (this->dominio.find("www.") == 0) {
        this->dominio = this->dominio.substr(4);
    }

    // PASO 2: Limpia el grafo destino
    grafoDestino.limpiar();  // ← asumo que tienes un método limpiar() en GrafoWeb
                             // si no, implementa: adyacencias.clear();

    // Estructura BFS
    std::queue<std::pair<std::string, int>> paginasPendientes;
    std::unordered_set<std::string> paginasVisitadas;

    // Inicialización del nodo raíz
    paginasPendientes.push({urlInicial, 0});
    paginasVisitadas.insert(urlInicial);

    grafoDestino.agregarNodo(urlInicial);  // ← importante: agregamos el nodo inicial

    grafo.agregarNodo(urlInicial); //inicializa el grafo con la url inicial
    
    int visitadasCount = 1; //contador de paginas visitadas
    //se inicializa en 1 porque la url inicial ya se cuenta como visitada

    while (!paginasPendientes.empty() && visitadasCount < maxPaginas) {
        auto [urlActual, profundidadActual] = paginasPendientes.front();
        paginasPendientes.pop();

        if (profundidadActual >= maxProfundidad) {
            continue;
        }

        // PASO 3: Descargar HTML
        std::string html = comunicacion.descargarPagina(urlActual);
        if (html.empty()) {
            continue;
        }

        // PASO 4: Extraer enlaces
        std::vector<std::string> enlaces = procesador.extraerEnlaces(html, urlActual);

            //verifica si el enlace ya fue visitado
            if(paginasVisitadas.find(enlace) == paginasVisitadas.end()){ // si no ha sido visitada
                paginasVisitadas.insert(enlace); // marca el enlace como visitado
                paginasPendientes.push(std::make_pair(enlace, profundidadActual + 1)); 
                //inserta el nuevo enlace en la cola con profundidad incrementada
                //al hacer esto incrementamos la profundidad para reflejar el nivel de exploracion
                
                grafo.agregarArista(urlActual, enlace); 

                // Agregamos al grafo usando los métodos de GrafoWeb
                grafoDestino.agregarArista(urlActual, enlace);
                grafoDestino.agregarNodo(enlace);  // por si no existe aún

                visitadasCount++;

                if (visitadasCount >= maxPaginas) {
                    break;
                }
            }
        }
    }

    std::cout << "DEBUG: Rastreo finalizado. Páginas agregadas: " 
              << grafoDestino.obtenerGrafo().size() << "\n";
}



