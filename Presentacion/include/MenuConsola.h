#ifndef MENUCONSOLA_H
#define MENUCONSOLA_H


#include <string>
#include <iostream>
#include "Negocio/include/WebCrawler.h"
#include "../include/ProcesadorEnlaces.h"
#include "../include/GrafoWeb.h"

class MenuConsola {

private:
    WebCrawler crawler;
    ComunicacionHTTP comunicacionHttp; 
    ProcesadorEnlaces procesadorEnlaces; 
    GrafoWeb grafoWeb; 


public: 

    void probarDescargarPagina(); 
    void probarExtraerEnlaces(); 
    void probarNormalizarUrl(); 
    void probarMismoDominio();
    void probarRastrear();


public:
    void ejecutar(); 

}; 

#endif // MENUCONSOLA_H






