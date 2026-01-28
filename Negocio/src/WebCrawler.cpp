#include "../include/WebCrawler.h" // Incluye la definición de la clase WebCrawler
#include "../include/ComunicacionHTTP.h" // Incluye la definición de la clase WebCrawler
#include "../include/ProcesadorEnlaces.h" // Incluye la definición de la clase WebCrawler


#include <iostream>   // Entrada / salida estándar


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
    ProcesadorEnlaces procesador(urlInicial); 
    ComunicacionHTTP comunicacion; 
    
    std::string dominioBase = procesador.getDominioBase(); 

    dominioBase = procesador.extraerDominio(urlInicial); //PASO 1: EXTRAE EL DOMINIO BASE
    //se obtiene el dominio base a partir de una url inicial 
    // este dominio se usara como criterio de restriccion para evitar enlaces externos
    // se establece una regla global del rastreo desde el inicio

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

        std::string html = comunicacion.descargarPagina(urlActual);  // PASO 3: SE DESCARGA EL CONTENIDO HTML DE LA PAGINA ACTUAL

        if(html.empty()){
            continue; //si no se pudo descargar, se omite
        }
        
        std::vector<std::string> enlaces = procesador.extraerEnlaces(html, urlActual);  // PASO 4: se extraen los enlaces del html descargado

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



