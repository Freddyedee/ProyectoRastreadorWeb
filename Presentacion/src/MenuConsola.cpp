#include "../include/MenuConsola.h" 
#include "../include/ProcesadorEnlaces.h"
#include "../include/ComunicacionHTTP.h"
#include "../include/GrafoWeb.h"
#include "../include/WebCrawler.h"

#include <iostream>
#include <limits>
#include <string>

void MenuConsola::ejecutar(){

    int opcion = 0;

    do {
        std::cout << "\n================================================================================";
        std::cout << "\n                  MENU PRINCIPAL - WEB CRAWLER DE PRUEBAS                  \n";
        std::cout << "================================================================================\n\n";

        std::cout << "Opciones disponibles:\n";
        std::cout << "[1] Probar descarga de pagina web\n";
        std::cout << "[2] Probar extraccion de enlaces\n";
        std::cout << "[3] Probar normalizacion de URL\n";
        std::cout << "[4] Probar verificacion de dominio\n";
        std::cout << "[5] Probar rastreo completo (BFS)\n";
        std::cout << "[0] Salir del programa\n\n";

        std::cout << "Seleccione una opcion (0-5): ";
        std::cin >> opcion;

        // Limpia buffer por si hay entrada invalida
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            opcion = -1;
        }

        std::cout << "\n";

        switch (opcion) {
            case 1: probarDescargarPagina(); break;
            case 2: probarExtraerEnlaces(); break;
            case 3: probarNormalizarUrl(); break;
            case 4: probarMismoDominio(); break;
            //case 5: probarRastrear(); break;   // Descomentado
            case 0:
                std::cout << "================================================================================\n";
                std::cout << "   ¡Gracias por usar el Web Crawler de pruebas!\n";
                std::cout << "   Programa finalizado.\n";
                std::cout << "================================================================================\n";
                break;
            default:
                std::cout << "Opcion invalida. Por favor, seleccione una opcion valida.\n\n";
                // Pausa rapida para que vea el mensaje de error
                std::cout << "Presione Enter para continuar...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
                break;
        }
    } while (opcion != 0);
}

// ======================
// 1. Descarga de pagina
// ======================

void MenuConsola::probarDescargarPagina() {
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Prueba: Descarga de pagina web\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    std::string url;
    std::cout << "Ingrese la URL completa (ej. https://www.example.com): ";
    std::cin >> url;

    try {
        std::string contenidoHtml = comunicacionHttp.descargarPagina(url);
        std::cout << "\nDescarga exitosa.\n";
        std::cout << "Tamano del contenido HTML: " << contenidoHtml.size() << " bytes\n";
        std::cout << "La pagina se descargo correctamente y esta lista para procesamiento.\n";

    } catch (const std::exception& e) {
        std::cerr << "Error al descargar la pagina: " << e.what() << std::endl;
    }

    std::cout << "\nPresione Enter para volver al menu principal...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}


// =========================
// 2. Extraccion de enlaces
// =========================

void MenuConsola::probarExtraerEnlaces() {
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Prueba: Extraccion de enlaces\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    std::string url, baseUrl;
    std::cout << "Ingrese URL de la pagina a analizar: ";
    std::cin >> url;
    std::cout << "Ingrese URL base (para resolver enlaces relativos): ";
    std::cin >> baseUrl;

    try {
        std::string html = comunicacionHttp.descargarPagina(url);
        std::vector<std::string> enlaces = procesadorEnlaces.extraerEnlaces(html, baseUrl);

        std::cout << "\nDescarga y analisis completados.\n";
        std::cout << "Enlaces encontrados (" << enlaces.size() << "):\n\n";

        for (size_t i = 0; i < enlaces.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << enlaces[i] << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error al extraer enlaces: " << e.what() << std::endl;
    }

    std::cout << "\nPresione Enter para volver al menu principal...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}


// ========================
// 3. Normalizacion de URL
// ========================

void MenuConsola::probarNormalizarUrl() {
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Prueba: Normalizacion de URL\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Este proceso convierte rutas relativas en direcciones completas.\n\n";

    std::string url, baseUrl;
    std::cout << "1. Ingrese el fragmento de enlace (ej. /about o about.html): ";
    std::cin >> url;
    std::cout << "2. Ingrese la pagina base (ej. https://www.example.com): ";
    std::cin >> baseUrl;

    try {
        std::string urlNormalizada = procesadorEnlaces.normalizarURL(url, baseUrl);
        std::cout << "\nDireccion final generada:\n";
        std::cout << urlNormalizada << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error al normalizar URL: " << e.what() << std::endl;
    }

    std::cout << "\nPresione Enter para volver al menu principal...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}


// ===========================
// 4. Verificacion de dominio
// ===========================

void MenuConsola::probarMismoDominio() {
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Prueba: Verificacion de dominio\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    std::string urlInicial, urlEvaluar;

    std::cout << "Ingrese URL inicial (para establecer el dominio): ";
    std::cin >> urlInicial;
    crawler.rastrear(urlInicial, 0, 1);  // Solo para fijar dominio

    std::cout << "Ingrese URL a evaluar: ";
    std::cin >> urlEvaluar;

    if (procesadorEnlaces.esMismoDominio(urlEvaluar))
        std::cout << "\nPertenece al mismo dominio.\n";
    else
        std::cout << "\nNO pertenece al mismo dominio.\n";

    std::cout << "\nPresione Enter para volver al menu principal...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// =========================
// 5. Rastreo completo (BFS)
// =========================

void MenuConsola::probarRastrear() {
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Prueba: Rastreo completo (BFS)\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    std::string url;
    int profundidad, maxPaginas;

    std::cout << "URL inicial: ";
    std::cin >> url;
    std::cout << "Profundidad maxima: ";
    std::cin >> profundidad;
    std::cout << "Maximo de paginas: ";
    std::cin >> maxPaginas;

    std::cout << "\nIniciando rastreo...\n";
    crawler.rastrear(url, profundidad, maxPaginas);

    const auto& grafo = grafoWeb.obtenerGrafo();

    std::cout << "Rastreo completado.\n";
    std::cout << "Paginas descubiertas: " << grafo.size() << "\n\n";

    std::cout << "================================================================================\n";
    std::cout << "                       GRAFO DE ENLACES GENERADO                       \n";
    std::cout << "================================================================================\n\n";

    for (const auto& nodo : grafo) {
        std::cout << "Nodo: " << nodo.first << " (" << nodo.second.size() << " enlaces salientes)\n";
        for (const auto& enlace : nodo.second) {
            std::cout << "   -> " << enlace << "\n";
        }
        std::cout << "\n";
    }

    std::cout << "Presione Enter para volver al menu principal...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}