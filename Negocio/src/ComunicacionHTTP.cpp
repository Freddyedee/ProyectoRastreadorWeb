#include "../include/ComunicacionHTTP.h"
#include <stdexcept>

/**
 * @brief  Función callback utilizada por libcurl para almacenar los datos descargados desde una solicitud HTTP.
 *
 * @param contenido Puntero a los datos recibidos.
 * @param tamañoBloque Tamaño de cada bloque de datos.
 * @param numeroBloques Número de bloques recibidos.
 * @param userp Puntero al string donde se almacenará el contenido.
 * @return Número total de bytes procesados.
 *
 * @note Debe ser estática porque libcurl no trabaja con métodos
 *       asociados a instancias de clase.
 */

size_t ComunicacionHTTP::DatosAlmacenadosCurl(void* contenido, size_t tamannoBloque, size_t numeroBloques, std::string* userp) {
    
    //Calculo explicito del tamaño total de los datos recibidos
    size_t totalSize = tamannoBloque * numeroBloques;

    //conersion explicita del bloque generico de caracteres
    char* dataRecibida = static_cast<char*>(contenido);

    //Almacena los datos recibidos en el string proporcionado
    userp->append(dataRecibida, totalSize);

    //libcurl espera que la función retorne el número total de bytes procesados
    return totalSize;
}


/**
 * @brief Descaga el contenido HTML de una página web utilizando la libreria curl
 * 
 * Este metodo realiza  una solicitud http  a la url proporccionada y descarga 
 * el contenido html de la pagina. En caso de error lanza una excepcion.
 *  
 * @param url Direccion URL completa de la pgina a descargar  
 * @return std::string Contenido HTML de la página como string.
 * 
 * @throws std::runtime_error Si la URL no es accesible o la conexión falla.
 */

std::string ComunicacionHTTP::descargarPagina(const std::string& url) {
   
    //Variable para almacenar el contenido descargado HTML
    std::string contenidoPagina;

    //Inicializa una sesión CURL
    CURL* curl = curl_easy_init();

    //verifica que la inicialización fue exitosa
    if (curl == nullptr) {
        throw std::runtime_error("Error al inicializar CURL");
    }

    //Configura las opciones de CURL

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Establece la URL a descargar

    // Asociacion del callback de escritura 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ComunicacionHTTP::DatosAlmacenadosCurl);

    // Asociacion del buffer de destino 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &contenidoPagina);

    // Definicion de uin tiempo de espera para la operacion
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8L); // 8 segundos de timeout

    //ejecuta la solicitud HTTP
    CURLcode ejecucionHttp = curl_easy_perform(curl); 

    //liberación de recursos de CURL
    curl_easy_cleanup(curl);

    //verifar si la solicitud fue exitosa
    if (ejecucionHttp != CURLE_OK) {
        throw std::runtime_error("Error al descargar la página: " + url + " - " + curl_easy_strerror(ejecucionHttp));
    }

    return contenidoPagina; // Retorna el contenido HTML descargado

}

