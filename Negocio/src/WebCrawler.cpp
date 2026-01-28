#include "../include/WebCrawler.h" // Incluye la definición de la clase WebCrawler

#include <iostream>   // Entrada / salida estándar
#include <algorithm>

/**
 * @brief Extrae el dominio principal de una URL.
 *
 * este metodo obtiene eñ dominio principal de una url (sin protocolo ni ruta)
 * 
 * @param url URL completa.
 * @return Dominio sin protocolo ni ruta.
 */
std::string extraerDominio(const std::string& url) {
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

/**
 * @brief  Función callback utilizada por libcurl para almacenar los datos descargados desde una solicitud HTTP.
 *
 * @param contenido Puntero a los datos recibidos.
 * @param tamanoBloque Tamaño de cada bloque de datos.
 * @param numeroBloques Número de bloques recibidos.
 * @param userp Puntero al string donde se almacenará el contenido.
 * @return Número total de bytes procesados.
 *
 * @note Debe ser estática porque libcurl no trabaja con métodos
 *       asociados a instancias de clase.
 */

size_t WebCrawler::writeCallback(void* contenido, size_t tamanoBloque, size_t numeroBloques, std::string* userp) {
    
    //Calculo explicito del tamaño total de los datos recibidos
    size_t totalSize = tamanoBloque * numeroBloques;

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

std::string WebCrawler::descargarPagina(const std::string& url) {
   
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WebCrawler::writeCallback);

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

std::vector<std::string> WebCrawler::extraerEnlaces(const std::string& html, const std::string& base_url) {
    // Actualizamos el dominio base cada vez (por si cambia en redirecciones)
    this->dominio = extraerDominio(base_url);

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

std::string WebCrawler::normalizarURL(const std::string& url, const std::string& base_url) {
   
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
}


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
void WebCrawler::rastrear(const std::string& urlInicial, int maxProfundidad, int maxPaginas) {


    // inicializa el dominio base

    dominio = extraerDominio(urlInicial); //PASO 1: EXTRAE EL DOMINIO BASE
    //se obtiene el dominio base a partir de una url inicial 
    // este dominio se usara como criterio de restriccion para evitar enlaces externos
    // se establece una regla global del rastreo desde el inicio
    if (this->dominio.find("www.") == 0) {
    this->dominio = this->dominio.substr(4);  // quita "www."
    }


    //limpia el grafo previo
    grafo.clear(); // PASO 2: LIMPIA EL GRAFO ANTERIOR

    //Estructura para el rastreo BFS
    std::queue<std::pair<std::string, int>> paginasPendientes; // Cola para el algoritmo BFS

    std::unordered_set<std::string> paginasVisitadas; //para evitar visitas repetidas
    //almacena las urls que ya han sido visitadas. 
    //underorder_set ofrece busqueda promedio O(1). 

    //inicializacion dl nodo raiz 
    paginasPendientes.push(std::make_pair(urlInicial, 0)); //encola la url inicial con profundidad 0 
    paginasVisitadas.insert(urlInicial); // marca la url inicial como visitada 
    //se evita que el nodo raiz sea revistado. 

    grafo[urlInicial] = std::vector<std::string>(); //inicializa el grafo con la url inicial
    
    int visitadasCount = 1; //contador de paginas visitadas
    //se inicializa en 1 porque la url inicial ya se cuenta como visitada

    while(!paginasPendientes.empty() && visitadasCount < maxPaginas){ //el bucle while continua mientras existan paginas pendientes y no se haya alcanzado el maximo de paginas

        std::pair<std::string, int> paginaActual = paginasPendientes.front(); // extrae la url mas antigua de la cola
        paginasPendientes.pop();  //Se extrae la url mas antigua de la cola

        std::string urlActual = paginaActual.first; //url actual a procesar
        int profundidadActual = paginaActual.second; // profundidad actual
        // se descompone el par en url y profundidad

        if(profundidadActual >= maxProfundidad){
            continue; //si se alcanza la profundidad maxima, se omite la exploracion de enlaces
        }

        std::string html = descargarPagina(urlActual);  // PASO 3: SE DESCARGA EL CONTENIDO HTML DE LA PAGINA ACTUAL

        if(html.empty()){
            continue; //si no se pudo descargar, se omite
        }
        
        std::vector<std::string> enlaces = extraerEnlaces(html, urlActual);  // PASO 4: se extraen los enlaces del html descargado

        for(size_t i = 0; i < enlaces.size(); i++){
            std::string enlace = enlaces[i];
            //iteracion sobre los enlaces extraidos

            //verifica si el enlace ya fue visitado
            if(paginasVisitadas.find(enlace) == paginasVisitadas.end()){ // si no ha sido visitada
                paginasVisitadas.insert(enlace); // marca el enlace como visitado
                paginasPendientes.push(std::make_pair(enlace, profundidadActual + 1)); 
                //inserta el nuevo enlace en la cola con profundidad incrementada
                //al hacer esto incrementamos la profundidad para reflejar el nivel de exploracion
                
                grafo[urlActual].push_back(enlace);   //se agrega el enlace al grafo como nodo hijo de la pagina actual
                grafo[enlace] = std::vector<std::string>(); //inicializa el nodo del grafo para el nuevo enlace

                visitadasCount++; // Incrementa el contador de páginas visitadas
            
            }

                if(visitadasCount >= maxPaginas){
                    break; //si se alcanza el maximo de paginas, se detiene la exploracion
                }
            }
        }
    }

/**
 * @brief  Devuelve el grafo de enlaces generado por el rastreo.
 * 
 * @return const std::unordered_map<std::string, std::vector<std::string>>& Grafo de enlaces.
 */

const std::unordered_map<std::string, std::vector<std::string>>& WebCrawler::getGrafo() const {
    return grafo;
}

bool WebCrawler::esMismoDominio(const std::string& url) {
    std::string host = extraerDominio(url);
    if (host.empty()) return false;

    std::string hostLower = host;
    std::transform(hostLower.begin(), hostLower.end(), hostLower.begin(), ::tolower);

    std::string baseLower = dominio;
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