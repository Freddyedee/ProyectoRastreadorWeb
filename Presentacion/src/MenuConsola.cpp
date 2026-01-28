#include "../include/MenuConsola.h" 
#include "../include/ProcesadorEnlaces.h"
#include "../include/ComunicacionHTTP.h"
#include "../include/GrafoWeb.h"
#include "../include/WebCrawler.h"

#include <iostream>

void MenuConsola::ejecutar(){

    int opcion = 0; 

   do {
        std::cout << "\n=== MENU DE PRUEBAS DEL WEB CRAWLER ===\n";
        std::cout << "1. Probar descarga de pagina\n";
        std::cout << "2. Probar extraccion de enlaces\n";
        std::cout << "3. Probar normalizacion de URL\n";
        std::cout << "4. Probar verificacion de dominio\n";
        std::cout << "5. Probar rastreo completo (BFS)\n";
        std::cout << "0. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1: probarDescargarPagina(); break;
            case 2: probarExtraerEnlaces(); break;
            case 3: probarNormalizarUrl(); break;
            case 4: probarMismoDominio(); break;
            //case 5: probarRastrear(); break;
        }

    } while (opcion != 0);

}


void MenuConsola::probarDescargarPagina(){

   

    std::string url; 
    std::cout << "Ingrese url"; 
    std::cin >> url;

    try{

        std::string contenidoHtml = comunicacionHttp.descargarPagina(url);
        std::cout << "Contenido HTML descargado:\n" << std::endl; 
        std::cout << "Tamaño del contenido: " << contenidoHtml.size() << " bytes\n";

    } catch (const std::exception& e){

        std::cerr << "Error al descargar la pagina: " << e.what() << std::endl; 

    }
}

void MenuConsola::probarExtraerEnlaces(){

    
     
    std::string url;
    std::string baseUrl; 

    std::cout << "Ingrese URL: ";
    std::cin >> url;

    std::cout << "Ingrese url base: ";
    std::cin >> baseUrl;

    try {
        std::string html = comunicacionHttp.descargarPagina(url);
        std::vector<std::string> enlaces = procesadorEnlaces.extraerEnlaces(html, baseUrl);

        std::cout << "Enlaces extraidos:\n";
        for (const auto& enlace : enlaces) {
            std::cout << enlace << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error al extraer enlaces: " << e.what() << std::endl;
    }

}


void MenuConsola::probarNormalizarUrl(){


    std::string url;
    std::string baseUrl; 
    std::cout << "=====Este proceso convierte rutas cortas en direcciones completas.=====\n";
    std::cout << "1. Ingrese el fragmento de enlace: ";
    std::cin >> url;

    std::cout << "2. Ingrese la pagina donde esta ese enlace: ";
    std::cin >> baseUrl;

    try {
        std::string urlNormalizada = procesadorEnlaces.normalizarURL(url, baseUrl);
        std::cout << "Direccion final generada: " << urlNormalizada << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error al normalizar URL: " << e.what() << std::endl;
    }
}

void MenuConsola::probarMismoDominio(){
    
    std::string urlInicial, urlEvaluar;

    std::cout << "Ingrese URL inicial: ";
    std::cin >> urlInicial;

    crawler.rastrear(urlInicial, 0, 1); // solo para fijar dominio

    std::cout << "Ingrese URL a evaluar: ";
    std::cin >> urlEvaluar;

    if (procesadorEnlaces.esMismoDominio(urlEvaluar))
        std::cout << "Pertenece al mismo dominio\n";
    else
        std::cout << "NO pertenece al mismo dominio\n";
}

void MenuConsola::probarRastrear() {
    std::string url;
    int profundidad, maxPaginas;

    std::cout << "URL inicial: ";
    std::cin >> url;
    std::cout << "Profundidad maxima: ";
    std::cin >> profundidad;
    std::cout << "Maximo de paginas: ";
    std::cin >> maxPaginas;

    crawler.rastrear(url, profundidad, maxPaginas);

    const auto& grafo = grafoWeb.obtenerGrafo();

    std::cout << "\n=== GRAFO DE ENLACES ===\n";
    for (const auto& nodo : grafo) {
        std::cout << "Pagina: " << nodo.first << "\n";
        for (const auto& enlace : nodo.second) {
            std::cout << "  -> " << enlace << "\n";
        }
        std::cout << "------------------------\n";
    }
}
