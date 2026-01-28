#ifndef COMUNICACIONHTTP_H
#define COMUNICACIONHTTP_H

#include <iostream>
#include <string>
#include <curl/curl.h> // biblioteca para realizar solicitudes HTTP y descarga de contenido de paginas web

class ComunicacionHTTP{

    public: 

    ComunicacionHTTP() = default; 
    
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

    private: 

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
        static size_t DatosAlmacenadosCurl(void* contenido, size_t tamannoBloque, size_t numeroBloques, std::string* userp);  //funcion estatica para manejar la respuesta de curl

};

#endif //COMUNICACIONHTTP_H