#include "../include/ProcesadorEnlaces.h"
#include <iostream>

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
    // Actualizamos el dominio base cada vez (por si cambia en redirecciones)
    this->dominioBase = extraerDominio(base_url);

    std::unordered_set<std::string> enlacesUnicos;  // Evitamos duplicados
    size_t posicion = 0;

    while (true) {
        // Buscamos <a ... href=... (ignoramos mayúsculas/minúsculas)
        size_t tagInicio = html.find("<a", posicion);
        if (tagInicio == std::string::npos) break;

        // Buscamos href= dentro del tag <a ...>
        size_t hrefPos = html.find("href=", tagInicio);
        if (hrefPos == std::string::npos || hrefPos > html.find(">", tagInicio)) {
            posicion = tagInicio + 2;  // Avanzamos para evitar bucle infinito
            continue;
        }

        hrefPos += 5;  // Saltamos "href="

        // Saltamos espacios después de href=
        while (hrefPos < html.size() && html[hrefPos] == ' ') ++hrefPos;
        if (hrefPos >= html.size()) break;

        // Comilla de apertura: " o '
        char comilla = html[hrefPos];
        if (comilla != '"' && comilla != '\'') {
            posicion = hrefPos + 1;
            continue;
        }

        ++hrefPos;  // Saltamos la comilla de apertura

        // Buscamos la comilla de cierre
        size_t finUrl = html.find(comilla, hrefPos);
        if (finUrl == std::string::npos) break;

        std::string urlExtraida = html.substr(hrefPos, finUrl - hrefPos);

        // Validaciones básicas
        if (urlExtraida.empty() ||
            urlExtraida[0] == '#' ||                        // Anclas internas
            urlExtraida.find("mailto:") != std::string::npos ||
            urlExtraida.find("tel:") != std::string::npos ||
            urlExtraida.find("javascript:") != std::string::npos) {
            posicion = finUrl + 1;
            continue;
        }

        // Filtramos recursos no-HTML y externos que no aportan al grafo
        std::string lowerUrl = urlExtraida;
        std::transform(lowerUrl.begin(), lowerUrl.end(), lowerUrl.begin(), ::tolower);

        // Extensiones comunes que ignoramos
        std::unordered_set<std::string> extensionesNoHtml = {
            ".js", ".css", ".png", ".jpg", ".jpeg", ".gif", ".svg", ".woff", ".woff2", ".ttf",
            ".pdf", ".ico", ".eot", ".otf", ".mp3", ".mp4", ".zip", ".rar", ".exe"
        };

        size_t ultimoPunto = lowerUrl.find_last_of('.');
        if (ultimoPunto != std::string::npos) {
            std::string ext = lowerUrl.substr(ultimoPunto);
            if (extensionesNoHtml.count(ext)) {
                posicion = finUrl + 1;
                continue;
            }
        }

        // También ignoramos dominios externos de fuentes/CDN
        if (lowerUrl.find("fonts.googleapis") != std::string::npos ||
            lowerUrl.find("cdn.jsdelivr") != std::string::npos ||
            lowerUrl.find("cdnjs.cloudflare") != std::string::npos ||
            lowerUrl.find("ajax.googleapis") != std::string::npos ||
            lowerUrl.find("stackpath.bootstrapcdn") != std::string::npos) {
            posicion = finUrl + 1;
            continue;
        }

        // Normalizamos y verificamos dominio
        std::string urlNormalizada = normalizarURL(urlExtraida, base_url);
        if (!urlNormalizada.empty()) {
            std::cout << "DEBUG: URL normalizada: " << urlNormalizada << "\n";
            if (esMismoDominio(urlNormalizada)) {
                std::cout << "DEBUG: ACEPTADO (mismo dominio)\n";
                enlacesUnicos.insert(urlNormalizada);
            } else {
                std::cout << "DEBUG: RECHAZADO (dominio diferente)\n";
            }
        }
        // Avanzamos después de la comilla de cierre
        posicion = finUrl + 1;
    }

    // Convertimos el set a vector para devolver
    return {enlacesUnicos.begin(), enlacesUnicos.end()};
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
    std::string host = extraerDominio(url);
    if (host.empty()) return false;

    std::string hostLower = host;
    std::transform(hostLower.begin(), hostLower.end(), hostLower.begin(), ::tolower);

    std::string baseLower = dominioBase;
    std::transform(baseLower.begin(), baseLower.end(), baseLower.begin(), ::tolower);

    // Quitamos "www." del base si lo tiene (por seguridad)
    if (baseLower.find("www.") == 0) {
        baseLower = baseLower.substr(4);
    }

    if (hostLower == baseLower) return true;

    std::string sufijo = "." + baseLower;
    if (hostLower.size() > sufijo.size() &&
        hostLower.compare(hostLower.size() - sufijo.size(), sufijo.size(), sufijo) == 0) {
        return true;
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
    if (url.empty()) return "";

    size_t inicio = url.find("://");
    if (inicio == std::string::npos) {
        // Sin protocolo → asumimos que es solo dominio o path
        inicio = 0;
    } else {
        inicio += 3;  // Saltamos ://
    }

    size_t fin = url.find('/', inicio);
    if (fin == std::string::npos) {
        fin = url.size();
    }

    std::string dominio = url.substr(inicio, fin - inicio);

    // Quitamos puerto si existe (ej: :8080)
    size_t puerto = dominio.find(':');
    if (puerto != std::string::npos) {
        dominio = dominio.substr(0, puerto);
    }

    // Convertimos a minúsculas
    std::transform(dominio.begin(), dominio.end(), dominio.begin(), ::tolower);

    return dominio;
}

