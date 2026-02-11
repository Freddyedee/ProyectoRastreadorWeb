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
    if (url.empty()) {
        return "";
    }

    // 1. Limpiar espacios en blanco al inicio y final
    size_t start = url.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = url.find_last_not_of(" \t\n\r");
    std::string rel = url.substr(start, end - start + 1);

    // Si es absoluta, retornamos directamente después de limpiarla
    if (rel.find("http://") == 0 || rel.find("https://") == 0) {
        return limpiarURLAbsoluta(rel);
    }

    // 2. Caso protocolo relativo: //dominio/...
    if (rel.size() >= 2 && rel.substr(0, 2) == "//") {
        return limpiarURLAbsoluta("https:" + rel);
    }

    // 3. Si no hay base_url válida, devolvemos el relativo tal cual
    if (base_url.empty() || base_url.find("://") == std::string::npos) {
        return rel;
    }

    // 4. Extraemos componentes de la base_url
    size_t scheme_end = base_url.find("://");
    std::string scheme = base_url.substr(0, scheme_end + 3);  // http:// o https://

    size_t authority_start = scheme_end + 3;
    size_t path_start = base_url.find('/', authority_start);
    if (path_start == std::string::npos) path_start = base_url.size();

    std::string authority = base_url.substr(authority_start, path_start - authority_start);
    std::string base_path = (path_start < base_url.size()) ? base_url.substr(path_start) : "/";

    // 5. Resolvemos el path relativo
    std::string result_path;

    if (rel.empty()) {
        result_path = base_path;
    }
    else if (rel[0] == '/') {
        // Ruta absoluta desde la raíz
        result_path = rel;
    }
    else {
        // Ruta relativa: combinamos con el path base
        size_t last_slash = base_path.find_last_of('/');
        if (last_slash != std::string::npos) {
            result_path = base_path.substr(0, last_slash + 1) + rel;
        } else {
            result_path = "/" + rel;
        }
    }

    // 6. Resolvemos los segmentos . y .. (dot segments)
    std::vector<std::string> segments;
    size_t pos = 0;
    while (pos < result_path.size()) {
        size_t next = result_path.find('/', pos);
        if (next == std::string::npos) next = result_path.size();

        std::string segment = result_path.substr(pos, next - pos);

        if (segment == "." || segment.empty()) {
            // Ignorar . y segmentos vacíos
        }
        else if (segment == "..") {
            if (!segments.empty()) {
                segments.pop_back();
            }
        }
        else {
            segments.push_back(segment);
        }

        pos = next + 1;
    }

    // 7. Reconstruimos el path limpio
    std::string clean_path = "/";
    for (size_t i = 0; i < segments.size(); ++i) {
        clean_path += segments[i];
        if (i < segments.size() - 1 || !segments.empty()) {
            clean_path += "/";
        }
    }
    // Si solo queda /, lo dejamos como raíz
    if (clean_path == "/") clean_path = "/";

    // 8. Reconstruimos la URL completa
    std::string normalized = scheme + authority + clean_path;

    // 9. Aplicamos la limpieza final (minúsculas en host, quitar #, ordenar query, etc.)
    return limpiarURLAbsoluta(normalized);
}

// Función auxiliar privada (puedes declararla en el .h como private o inline)
std::string ProcesadorEnlaces::limpiarURLAbsoluta(std::string url) {
    if (url.empty()) return "";

    // a) Convertir esquema a minúsculas (http → HTTP no es común, pero por si acaso)
    size_t scheme_end = url.find("://");
    if (scheme_end != std::string::npos) {
        std::transform(url.begin(), url.begin() + scheme_end,
                       url.begin(), ::tolower);
    }

    // b) Quitar fragmento (#...)  →  casi siempre se ignora en crawlers
    size_t fragment_pos = url.find('#');
    if (fragment_pos != std::string::npos) {
        url.erase(fragment_pos);
    }


    // c) Opcional: ordenar parámetros de query (muy recomendado para deduplicar)
    size_t query_pos = url.find('?');
    if (query_pos != std::string::npos) {
        std::string path = url.substr(0, query_pos);
        std::string query = url.substr(query_pos + 1);

        // Separar parámetros
        std::vector<std::string> params;
        size_t last = 0;
        size_t pos = query.find('&');
        while (pos != std::string::npos) {
            params.push_back(query.substr(last, pos - last));
            last = pos + 1;
            pos = query.find('&', last);
        }
        if (last < query.size()) {
            params.push_back(query.substr(last));
        }

        // Ordenar alfabéticamente
        std::sort(params.begin(), params.end());

        // Reconstruir query
        std::string new_query;
        for (size_t i = 0; i < params.size(); ++i) {
            if (!new_query.empty()) new_query += "&";
            new_query += params[i];
        }

        url = path + "?" + new_query;
    }

    // f) Convertir host a minúsculas 
    size_t host_start = url.find("://");
    if (host_start != std::string::npos) {
        host_start += 3;
        size_t host_end = url.find('/', host_start);
        if (host_end == std::string::npos) {
            host_end = url.find('?', host_start);
        }
        if (host_end == std::string::npos) {
            host_end = url.find('#', host_start);
        }
        if (host_end == std::string::npos) {
            host_end = url.size();
        }

        std::string host = url.substr(host_start, host_end - host_start);
        std::transform(host.begin(), host.end(), host.begin(), ::tolower);

        // Reconstruir URL con host en minúsculas
        url = url.substr(0, host_start) + host + url.substr(host_end);
    }

    return url;
}

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
    if (url.empty()) return "";

    size_t inicio = url.find("://");
    if (inicio == std::string::npos) {
        inicio = 0;
    } else {
        inicio += 3;  // Saltamos ://
    }

    size_t fin = url.find('/', inicio);
    if (fin == std::string::npos) {
        fin = url.size();
    }

    std::string dominio = url.substr(inicio, fin - inicio);

    // Quitamos puerto si existe (:8080)
    size_t puerto = dominio.find(':');
    if (puerto != std::string::npos) {
        dominio = dominio.substr(0, puerto);
    }

    // Quitamos "www." al inicio si existe
    if (dominio.size() > 4 && dominio.substr(0, 4) == "www.") {
        dominio = dominio.substr(4);
    }

    // Todo a minúsculas
    std::transform(dominio.begin(), dominio.end(), dominio.begin(), ::tolower);

    return dominio;
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

    if (url.find("${") != std::string::npos || url.find("<%") != std::string::npos) {
    return false;  // plantilla no resuelta
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

    return minuscula.find(".pdf") != std::string::npos ||
           minuscula.find(".css") != std::string::npos ||
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



