#include "../include/ProcesadorEnlaces.h"
#include <iostream>

/**
 * @brief  Analiza el contenido HTML de una página y extrae enlaces válidos.

 * 
 * Este método busca los atributos href en el hmll, extraw los valores
 * de los enlaces y los valida. los enlaces relativos se resuelven a urls absolutas. 
 * los enlaces que no sean validos o pertencezcan al mismo dominio se omiten. 
 * 
 * @param html contenido HTML de la página web
 * @param base_url url base para resolver enlaces relativos
 * @return std::vector<std::string>  lista de URLs extraídas
 */

std::vector<std::string> ProcesadorEnlaces::extraerEnlaces(const std::string& html, const std::string& base_url) {


    dominioBase = extraerDominio(base_url); //asegura que el dominio base este actualizado

    std::vector<std::string> enlacesExtraidos; // vector que acutua como lista de los enlaces extraidos

    size_t posicion = 0;

    std::string buscarPatron = "href=\""; // Busca todos los enlaces <a href="..."> en el HTML

    while(true){

        size_t hrefPosicion = html.find(buscarPatron, posicion); // Busqueda del siguiente atributo href

        if(hrefPosicion == std::string::npos){
            break; // No se encontraron más enlaces
        }


        size_t urlInicio = hrefPosicion + buscarPatron.length(); //Posicion inicial del url 

        size_t urlFin = html.find("\"", urlInicio);  // Busqueda del final del enalace  

        if(urlFin == std::string::npos){
            break; // Formato HTML incorrecto
        }

        std::string urlExtraido = html.substr(urlInicio, urlFin - urlInicio); //Extraccion del url


        //validaciones basicas
        bool esValido = true; 

        if(urlExtraido.empty()) esValido = false;
        if(!urlExtraido.empty() && urlExtraido[0] == '#') esValido = false; // Enlaces de anclaje
        if(urlExtraido.find("mailto:") != std::string::npos) esValido = false;

        // Resolución de URLs relativas
        if(esValido){

            std::string normalizedUrl = normalizarURL(urlExtraido, base_url);

            if(esMismoDominio(normalizedUrl)){
                enlacesExtraidos.push_back(normalizedUrl); // Agrega el enlace si pertenece al mismo dominio
            }
        }

        posicion = urlFin + 1; // Avanza el cursor para buscar el siguiente enlace
        std::cout << "DEBUG: Encontrado href original: " << urlExtraido << std::endl;
    }
    return enlacesExtraidos;
};

/**
 * @brief * Convierte un enlace relativo o parcial en una URL completa.
 * 
 * Este metodo se encarga de convertir enlaces relativos a urls absolutas
 * si la  url es absoluta, se retonrna tal cual. si es relativa, se completa usando la url base.
 * 
 * @param url Enlace encontrado en el HTML.
 * @param base_url URL base de la página actual.
 * @return std::string URL absoluta normalizada.
 */

std::string ProcesadorEnlaces::normalizarURL(const std::string& url, const std::string& base_url) {
   
    //limpiar posibles espacios en blanco al ingresar el url 

    size_t inicio = url.find_first_not_of(" \t\n\r");
    if(inicio == std::string::npos){
        return ""; // La URL está vacía o solo contiene espacios en blanco
    }

    std::string urlSinEspacios = url.substr(inicio);
    // Caso 1: URL absoluta
    if(urlSinEspacios.find("http://") == 0 || urlSinEspacios.find("https://") == 0){
        return urlSinEspacios; 
    }

    // Caso 1.5: Protocol relative (empieza con //)
    if(urlSinEspacios.find("//") == 0){
        return "https:" + urlSinEspacios;
    }

    // Caso 2: Enlace relativo a la raíz (empieza con /)
    if(!urlSinEspacios.empty() && urlSinEspacios[0] == '/'){
        // Necesitamos el "host" base (ej: https://es.wikipedia.org)
        // Buscamos el tercer '/' después de https://
        size_t corte = base_url.find('/', 8); 
        std::string host = (corte == std::string::npos) ? base_url : base_url.substr(0, corte);
        return host + urlSinEspacios;
    }

    // Caso 3: Enlace relativo simple
    // Si base_url no termina en '/', añadirlo
    if (base_url.back() != '/') {
        return base_url + "/" + urlSinEspacios;
    }
    return base_url + urlSinEspacios;
}; 


bool ProcesadorEnlaces::esMismoDominio(const std::string& url) {
    try {
        std::string dominioExtraido = extraerDominio(url);
        
        // Verificamos si el dominio base está contenido dentro del dominio extraído
        // Ejemplo: "uneg.edu.ve" está dentro de "moodle.uneg.edu.ve"
        if (dominioExtraido.find(dominioBase) != std::string::npos) {
            return true;
        }
    } catch (...) {
        return false; // Si extraerDominio falla, no es un enlace válido para nosotros
    }
    return false;
}

/**
 * @brief Extrae el dominio principal de una URL.
 *
 * este metodo obtiene eñ dominio principal de una url (sin protocolo ni ruta)
 * 
 * @param url URL completa.
 * @return Dominio sin protocolo ni ruta.
 */
std::string ProcesadorEnlaces::extraerDominio(const std::string& url) {
   
    size_t posicionProtocolo =  url.find("://"); // Encuentra el protocolo (http:// o https://)

    if(posicionProtocolo == std::string::npos){
        throw std::invalid_argument("URL inválida: " + url);  // Si no se encuentra el protocolo, es una URL inválida
    
    }

    size_t inicioDominio = posicionProtocolo + 3; // Longitud de "://"
    size_t finDominio = url.find('/', inicioDominio);   // Encuentra el final del dominio

    if(finDominio == std::string::npos){
        return url.substr(inicioDominio); // El dominio es hasta el final de la URL
    }

    return url.substr(inicioDominio, finDominio - inicioDominio); // Extrae el dominio
}

