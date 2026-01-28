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

    public:  
        std::string dominio; //almacena el dominio base del URL inicial 
        std::unordered_map<std::string, std::vector<std::string>> grafo; //Representacion del grafo de paginas web, donde std::string -> url y vector<string> -> urls enlazadas
        ComunicacionHTTP almacenadorDatos;
        

        /**
         * @brief 
         * Procede a realizarse el proceso de rastreo utilizando BFS
         * @param urlInicial inicia desde dond comieza el rastreo
         * @param maxProfundidad profundidad maxima del rastreo
         * @param maxPaginas numero maximo de paginas a rastrear
         * @note Este método construye internamente el grafo de enlaces.

         */
        void rastrear(const std::string& urlInicial, int maxProfundidad, int maxPaginas, GrafoWeb& grafoDestino);
};

#endif // WEBCRAWLER_H