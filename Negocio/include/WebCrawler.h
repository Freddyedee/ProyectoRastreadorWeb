#ifndef WEBCRAWLER_H
#define WEBCRAWLER_H

#include <string> // permite el uso de objetos de cadena de texto
#include <curl/curl.h> // biblioteca para realizar solicitudes HTTP y descarga de contenido de paginas web
#include <vector> //almacenamiento de listas dinamicas de URLs
#include <unordered_map> // utilizado en la representacion del grafo, dobnde la clave es la URL y el valor es una lista de URLs enlazadas aristas. 
#include <unordered_set> // para evitar URLs duplicadas y controlador las URLs visitadas  
#include <queue> //principal herramienta para el algoritmo de BFS

// Clase que implementa un rastreador web (web crawler) para explorar y analizar páginas web.
class WebCrawler {

    public:  
        std::string dominio; //aalmacena el dominio base del URL inicial 
        std::unordered_map<std::string, std::vector<std::string>> grafo; //Representacion del grafo de paginas web, donde std::string -> url y vector<string> -> urls enlazadas

        /**
        * @brief 
        * Función callback utilizada por libcurl para almacenar los datos
        * descargados desde una solicitud HTTP.
        *
        * @param datosRecibidos Puntero a los datos recibidos.
        * @param tamañoBloque Tamaño de cada bloque de datos.
        * @param numeroBloques Número de bloques recibidos.
        * @param userp Puntero al string donde se almacenará el contenido.
        * @return Número total de bytes procesados.
        *
        * @note Debe ser estática porque libcurl no trabaja con métodos
        *       asociados a instancias de clase.
         * 
     */
        static size_t writeCallback(void* contenido, size_t tamañoBloque, size_t numeroBloques, std::string* userp);  //funcion estatica para manejar la respuesta de curl


        /**
     * @brief 
     * 
        * Descarga el contenido HTML de una página web.
        *
        * @param url Dirección completa de la página a descargar.
        * @return Contenido HTML de la página como string.
        *
        * @throws Puede fallar si la URL no es accesible o la conexión falla.
    */
        std::string descargarPagina(const std::string& url);

    
        /*
            @brief Extrae todos los enlaces a href desde el HTML de una página.
            @param html Código HTML de la página.
            @param base_url URL base para resolver enlaces relativos.
            @return Vector con las URLs encontradas.
        */
        std::vector<std::string> extraerEnlaces(const std::string& html, const std::string &base_url);

        /**
        * @brief 
        * 
        *  Verifica si una URL pertenece al mismo dominio base.    
        * 
        * @param url URL a evaluar . 
        * @return true si pertenece al dominio base.
        * @return false si no pertenece dentro del dominio base. 
    */

        bool esMismoDominio(const std::string& url);

        /**
     * @brief 
     * Extrae el dominio principal de una url
     * 
     * @param url URL completa 
     * @return std::string (Dominio extraído por ejemplo: "ejemplo.com")
     */
        std::string extraerDominio(const std::string& url);

        std::string normalizarURL(const std::string& url, const std::string& base_url);

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
        const std::unordered_map<std::string, std::vector<std::string>>& getGrafo() const; 



};









#endif // WEBCRAWLER_H