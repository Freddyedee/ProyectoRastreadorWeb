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
    std::string ultimaUrlRastreada;
   


public: 

    void probarDescargarPagina(); 
    void probarExtraerEnlaces(); 
    void probarNormalizarUrl(); 
    void probarMismoDominio();
    void probarRastrear();
    void probarAnalizarGrafo(); 
    void probarBuscarCamino(); 


public:
    void ejecutar(); 

}; 

#endif // MENUCONSOLA_H






