#ifndef WEBCRAWLER_H
#define WEBCRAWLER_H

#include <string> // permite el uso de objetos de cadena de texto
#include <vector> //almacenamiento de listas dinamicas de URLs
#include <unordered_map> // utilizado en la representacion del grafo, dobnde la clave es la URL y el valor es una lista de URLs enlazadas aristas. 
#include <unordered_set> // para evitar URLs duplicadas y controlador las URLs visitadas  
#include <queue> //principal herramienta para el algoritmo de BFS
#include "ComunicacionHTTP.h"
#include "GrafoWeb.h"

// Clase que implementa un rastreador web (web crawler) para explorar y analizar páginas web.
class WebCrawler {

private: 
    GrafoWeb grafo;
   
public:  
        std::string dominio; //aalmacena el dominio base del URL inicial 
        ComunicacionHTTP almacenadorDatos;
         
        /**
         * @brief 
         * Procede a realizarse el proceso de rastreo utilizando BFS
         * @param urlInicial inicia desde dond comieza el rastreo
         * @param maxProfundidad profundidad maxima del rastreo
         * @param maxPaginas numero maximo de paginas a rastrear
         * @note Este método construye internamente el grafo de enlaces.

         */
        void rastrear(const std::string& urlInicial, int maxProfundidad, int maxPaginas);

        /**
         * @brief Get the Grafo object
         * Devuelve el grafo construido por el rastreador web.
         * @return const std::unordered_map<std::string, std::vector<std::string>>& , referiencia constante al grafo.
         * 
         * @note Se retorna como referencia constante para evitar copias innecesarias y modificaciones externas.
         */
        const std::unordered_map<std::string, std::vector<std::string>>& getGrafo() const {
        return grafo.getGrafo();
    }


};

#endif // WEBCRAWLER_H