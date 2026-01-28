#include "../include/MenuConsola.h"
#include "../include/ProcesadorEnlaces.h"
#include "../include/ComunicacionHTTP.h"
#include "../include/GrafoWeb.h"
#include "../include/WebCrawler.h"

#include <iostream>
#include <vector>
#include <exception>


void MenuConsola::ejecutar(){

    int opcion = 0;

   do {
        std::cout << "\n=== MENU DE PRUEBAS DEL WEB CRAWLER ===\n";
        std::cout << "1. Probar descarga de pagina\n";
        std::cout << "2. Probar extraccion de enlaces\n";
        std::cout << "3. Probar normalizacion de URL\n";
        std::cout << "4. Probar verificacion de dominio\n";
        std::cout << "5. Probar rastreo completo (BFS)\n";
        std::cout << "6. Buscar camino a pagina con palabra clave\n";
        std::cout << "7. Calcular metricas estructurales del sitio\n";
        std::cout << "0. Salir\n";
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1: probarDescargarPagina(); break;
            case 2: probarExtraerEnlaces(); break;
            case 3: probarNormalizarUrl(); break;
            case 4: probarMismoDominio(); break;
            case 5: probarRastrear(); break;
            case 6: probarBuscarCaminoPalabraClave(); break;
            case 7: probarCalcularMetricas(); break;
        }

    } while (opcion != 0);

}


void MenuConsola::probarDescargarPagina(){

    std::string url;
    std::cout << "Ingrese url: ";
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

void MenuConsola::probarMismoDominio() {
    std::string urlInicial, urlEvaluar;

    std::cout << "Ingrese URL inicial (para obtener el dominio base): ";
    std::cin >> urlInicial;

    std::cout << "Ingrese URL a evaluar: ";
    std::cin >> urlEvaluar;

    try {
        // Extraemos el dominio base directamente (sin rastrear)
        std::string dominioBase = procesadorEnlaces.extraerDominio(urlInicial);

        if (dominioBase.empty()) {
            std::cout << "No se pudo extraer el dominio base de la URL inicial.\n";
            return;
        }

        std::cout << "Dominio base detectado: " << dominioBase << "\n";

        // Ahora evaluamos la segunda URL
        if (procesadorEnlaces.esMismoDominio(urlEvaluar)) {
            std::cout << "Pertenece al mismo dominio (o subdominio).\n";
        } else {
            std::cout << "NO pertenece al mismo dominio.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error al procesar dominios: " << e.what() << std::endl;
    }
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

    // Opcional: prueba de descarga (puedes quitarla si no la necesitas aquí)
    try {
        std::string contenidoHtml = comunicacionHttp.descargarPagina(url);
        std::cout << "Contenido HTML descargado (prueba):\n";
        std::cout << "Tamaño del contenido: " << contenidoHtml.size() << " bytes\n";
    } catch (const std::exception& e) {
        std::cerr << "Error al descargar la pagina (prueba): " << e.what() << std::endl;
    }

    // Realizamos el rastreo
    std::cout << "Iniciando rastreo...\n";
    crawler.rastrear(url, profundidad, maxPaginas, grafoWeb);

    // ¡Aquí creamos el AnalizadorGrafo con el grafo del crawler!
    delete analizador;  // Borramos si ya existía (evita memory leak)
    analizador = new AnalizadorGrafo(grafoWeb.obtenerGrafo());

    std::cout << "Rastreo completado. Grafo construido con " 
              << grafoWeb.obtenerGrafo().size() << " páginas.\n";

    // Mostramos el grafo (opcional, puedes comentarlo si es muy largo)
    std::cout << "\n=== GRAFO DE ENLACES ===\n";
    const auto& grafo = grafoWeb.obtenerGrafo();
    for (const auto& nodo : grafo) {
        std::cout << "Pagina: " << nodo.first << "\n";
        for (const auto& enlace : nodo.second) {
            std::cout << "  -> " << enlace << "\n";
        }
        std::cout << "------------------------\n";
    }
}

void MenuConsola::probarBuscarCaminoPalabraClave() {
    if (!analizador) {
        std::cout << "Primero debes realizar un rastreo completo (opcion 5).\n";
        return;
    }

    std::string paginaInicial;
    std::string palabraClave;

    std::cout << "Ingrese la pagina inicial (ej: https://www.uneg.edu.ve): ";
    std::cin >> paginaInicial;

    std::cout << "Ingrese la palabra clave a buscar en la URL: ";
    std::cin >> palabraClave;

    std::cout << "Buscando camino...\n";
    auto camino = analizador->buscarCaminoConPalabraClave(paginaInicial, palabraClave);

    analizador->imprimirCamino(camino);
}

void MenuConsola::probarCalcularMetricas() {
    if (!analizador) {
        std::cout << "Primero debes realizar un rastreo completo (opcion 5).\n";
        return;
    }

    std::string paginaInicial;
    std::cout << "Ingrese la pagina inicial para calcular metricas (ej: https://www.uneg.edu.ve): ";
    std::cin >> paginaInicial;

    std::cout << "Calculando metricas...\n";
    analizador->calcularMetricas(paginaInicial);
}