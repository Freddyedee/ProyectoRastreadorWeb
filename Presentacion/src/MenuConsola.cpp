#include "../include/MenuConsola.h"
#include "../include/ProcesadorEnlaces.h"
#include "../include/ComunicacionHTTP.h"
#include "../include/GrafoWeb.h"
#include "../include/WebCrawler.h"
#include "../proyectoEstructuraDatos/Datos/include/GestorArchivos.h"
#include "../include/AnalizadorGrafo.h"
#include "../include/BuscadorClave.h"

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
        std::cout << "[6] Analizar grafo generado\n";
        std::cout << "[7] Buscar camino por palabra clave\n";
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
            case 5: probarRastrear(); break;   
            case 6: probarAnalizarGrafo(); break;
            case 7: probarBuscarCamino(); break;

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

    // Obtener referencia al mapa real del grafo
    const auto& grafoMap = crawler.getGrafo();  // Usa un nombre claro

   std::cout << "Rastreo completado.\n";
    std::cout << "Paginas descubiertas: " << grafoMap.size() << "\n\n";

    std::cout << "================================================================================\n";
    std::cout << "                       GRAFO DE ENLACES GENERADO                       \n";
    std::cout << "================================================================================\n\n";

    std::cout << "================ GRAFO GENERADO ================\n\n";

    for (const auto& par : grafoMap) {
        std::cout << par.first << " -> ";
        if (par.second.empty()) {
            std::cout << "(sin enlaces salientes)";
        } else {
            for (const auto& enlace : par.second) {
                std::cout << enlace << " ";
            }
        }
        std::cout << "\n";
    }

    std::cout << "\nTotal de paginas en el grafo: " << grafoMap.size() << "\n";
    GestorArchivos::guardarGrafo(crawler.getGrafo(), "grafo.txt");
    std::cout << "Grafo guardado en grafo_uneg.txt\n";

    std::cout << "\nPresione Enter para volver al menu principal...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}


//funciones de prueba del inciso b y c

void MenuConsola::probarAnalizarGrafo() {
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Prueba: Analisis de grafo\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    if (crawler.getGrafo().empty()) {
        std::cout << "El grafo está vacío. Realice primero un rastreo.\n";
    } else {
        std::string paginaInicial;
        std::cout << "Ingrese URL inicial para calcular profundidad: ";
        std::cin >> paginaInicial;

        AnalizadorGrafo analizador(crawler.getGrafo(), ""); // Dominio vacío opcional
        analizador.calcularMetricas(paginaInicial);
    }

    std::cout << "\nPresione Enter para volver al menu principal...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void MenuConsola::probarBuscarCamino() {
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Prueba: Busqueda de camino por palabra clave\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    if (crawler.getGrafo().empty()) {
        std::cout << "El grafo está vacío. Realice primero un rastreo.\n";
    } else {
        std::string paginaInicial, palabraClave;
        std::cout << "Ingrese URL inicial: ";
        std::cin >> paginaInicial;
        std::cout << "Ingrese palabra clave a buscar en URLs: ";
        std::cin >> palabraClave;

        BuscadorClave buscador(crawler.getGrafo());
        std::vector<std::string> camino = buscador.buscarCaminoConPalabraClave(paginaInicial, palabraClave);
        buscador.imprimirCamino(camino);
    }

    std::cout << "\nPresione Enter para volver al menu principal...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
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