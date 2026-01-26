#ifndef MENUCONSOLA_H
#define MENUCONSOLA_H


#include <string>
#include <iostream>
#include "Negocio/include/WebCrawler.h"

class MenuConsola {

private:
    WebCrawler crawler;

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






