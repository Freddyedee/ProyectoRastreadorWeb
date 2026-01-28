#ifndef MENUCONSOLA_H
#define MENUCONSOLA_H


#include <string>
#include <iostream>
#include "Negocio/include/WebCrawler.h"
#include "Negocio/include/AnalizadorGrafo.h"

class MenuConsola {

private:
    WebCrawler crawler;
    AnalizadorGrafo* analizador = nullptr;  // Se crea después de rastrear

public: 

    void probarDescargarPagina(); 
    void probarExtraerEnlaces(); 
    void probarNormalizarUrl(); 
    void probarMismoDominio();
    void probarRastrear();

    void probarBuscarCaminoPalabraClave();
    void probarCalcularMetricas();


public:
    void ejecutar(); 

}; 

#endif // MENUCONSOLA_H






