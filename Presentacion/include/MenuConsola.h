#ifndef MENUCONSOLA_H
#define MENUCONSOLA_H


#include <string>
#include <iostream>
#include "Negocio/include/WebCrawler.h"
#include "Negocio/include/AnalizadorGrafo.h"
#include "Negocio/include/ComunicacionHTTP.h"
#include "Negocio/include/ProcesadorEnlaces.h"
#include "Negocio/include/GrafoWeb.h"

class MenuConsola {

private:
    WebCrawler crawler;
    AnalizadorGrafo* analizador = nullptr;  // Se crea después de rastrear
    ComunicacionHTTP comunicacionHttp; 
    ProcesadorEnlaces procesadorEnlaces; 
    std::string ultimaUrlRastreada;
   


public: 

    void probarDescargarPagina(); 
    void probarExtraerEnlaces(); 
    void probarNormalizarUrl(); 
    void probarMismoDominio();
    void probarRastrear();
    void probarAnalizarGrafo(); 
    void probarBuscarCamino(); 

    void probarBuscarCaminoPalabraClave();
    void probarCalcularMetricas();


public:
    void ejecutar(); 

}; 

#endif // MENUCONSOLA_H






