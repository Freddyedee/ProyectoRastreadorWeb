#include "../include/ProcesadorEnlaces.h"
#include <iostream>
#include <algorithm> 
ProcesadorEnlaces::ProcesadorEnlaces(const std::string& url) {
        dominioBase = extraerDominio(url);
        }

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
    dominioBase = extraerDominio(base_url);  // Actualiza dominio base
    std::vector<std::string> enlacesExtraidos;

    size_t pos = 0;

    while (pos < html.length()) {
        // === PASO 1: Buscar SOLO etiquetas <a o <A (ignora <link>, <script>, etc.) ===
        size_t aTagStart = html.find("<a", pos);
        if (aTagStart == std::string::npos) {
            aTagStart = html.find("<A", pos);
            if (aTagStart == std::string::npos) break;
        }

        // === PASO 2: Dentro de esta etiqueta <a, buscar href= ===
        size_t hrefPos = html.find("href", aTagStart);
        if (hrefPos == std::string::npos || hrefPos > html.find('>', aTagStart)) {
            pos = aTagStart + 2;
            continue;
        }

        size_t eqPos = html.find('=', hrefPos);
        if (eqPos == std::string::npos) {
            pos = hrefPos + 4;
            continue;
        }

        // Saltar espacios después del =
        size_t quoteStart = eqPos + 1;
        while (quoteStart < html.length() && std::isspace(html[quoteStart])) ++quoteStart;

        if (quoteStart >= html.length()) break;

        char quote = html[quoteStart];
        if (quote != '"' && quote != '\'') {
            pos = eqPos + 1;
            continue;
        }

        size_t quoteEnd = html.find(quote, quoteStart + 1);
        if (quoteEnd == std::string::npos) break;

        std::string urlRaw = html.substr(quoteStart + 1, quoteEnd - quoteStart - 1);

        pos = quoteEnd + 1;  // Avanzar para el siguiente

        // === PASO 3: Usar tus métodos de filtro ===
        if (!esEnlaceValido(urlRaw)) continue;
        if (esRecursoEstatico(urlRaw)) continue;

        std::string normalized = normalizarURL(urlRaw, base_url);

        // Usa tu método perteneceAlDominio o esMismoDominio
        if (esMismoDominio(normalized)) {
            enlacesExtraidos.push_back(normalized);
        }
    }

    return enlacesExtraidos;
}

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
        std::string dominioExtraido = normalizarDominio(extraerDominio(url));
        std::string dominioBaseNormalizado = normalizarDominio(dominioBase);

        // Exactamente el mismo dominio
        if (dominioExtraido == dominioBaseNormalizado) {
            return true;
        }

        // Subdominios válidos (moodle.uneg.edu.ve, servicio.uneg.edu.ve, etc.)
        if (dominioExtraido.size() > dominioBaseNormalizado.size() &&
            dominioExtraido.find("." + dominioBaseNormalizado) != std::string::npos) {
            return true;
        }

        return false;
    }
    catch (...) {
        return false;
    }
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

std::string ProcesadorEnlaces::normalizarDominio(const std::string& dominio) {
    // Elimina www. si existe
    if (dominio.find("www.") == 0) {
        return dominio.substr(4);
    }
    return dominio;
}


/**
 * @brief Verifica si un enlace es válido (navegable y no vacío ni especial)
 */
bool ProcesadorEnlaces::esEnlaceValido(const std::string& url) {
    if (url.empty()){
        return false;
    }
    if (url[0] == '#') {
        return false;
    }
    
    std::string minuscula = url;
    std::transform(minuscula.begin(), minuscula.end(), minuscula.begin(), ::tolower);

    if (minuscula.find("javascript:") == 0) return false;
    if (minuscula.find("mailto:") != std::string::npos) return false;
    if (minuscula.find("tel:") != std::string::npos) return false;

    return true;
}

/**
 * @brief Filtra recursos estáticos comunes (CSS, JS, imágenes)
 */
bool ProcesadorEnlaces::esRecursoEstatico(const std::string& url) {
    std::string minuscula = url;
    std::transform(minuscula.begin(), minuscula.end(), minuscula.begin(), ::tolower);

    return minuscula.find(".css") != std::string::npos ||
           minuscula.find(".js") != std::string::npos ||
           minuscula.find(".ico") != std::string::npos ||
           minuscula.find(".png") != std::string::npos ||
           minuscula.find(".jpg") != std::string::npos;
}

/**
 * @brief Verifica si la URL pertenece al dominio base
 */
bool ProcesadorEnlaces::perteneceAlDominio(const std::string& url) {
    return extraerDominio(url) == dominioBase;
}



