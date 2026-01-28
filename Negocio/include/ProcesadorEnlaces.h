#ifndef PROCESADORENLACES_H
#define PROCESADORENLACES_H

#include <string>
#include <vector>

class ProcesadorEnlaces {

    public: 

        ProcesadorEnlaces::ProcesadorEnlaces(const std::string& url) {
        dominioBase = extraerDominio(url);
        }

        explicit ProcesadorEnlaces(const std::string& dominio); 

         /**
            @brief Extrae todos los enlaces a href desde el HTML de una página.
            @param html Código HTML de la página.
            @param base_url URL base para resolver enlaces relativos.
            @return Vector con las URLs encontradas.
          */
        std::vector<std::string> extraerEnlaces(const std::string& html, const std::string &base_url);

        /**
         * @brief 
         * 
         * @param url 
         * @param base_url 
         * @return std::string 
         */
        std::string normalizarURL(const std::string& url, const std::string& base_url);

         /**
        * @brief 
        * 
        *  Verifica si una URL pertenece al mismo dominio base.    
        * 
        * @param url URL a evaluar . 
        * @return true si pertenece al dominio base.
        * @return false si no pertenece dentro del dominio base. 
        */
        bool esMismoDominio(const std::string& url);

        std::string getDominioBase(){
            return this->dominioBase; 
        }; 
        
         /**
        * @brief 
        * Extrae el dominio principal de una url
        * 
        * @param url URL completa 
        * @return std::string (Dominio extraído por ejemplo: "ejemplo.com")
        */
        std::string extraerDominio(const std::string& url);

    private: 
       
        std::string dominioBase;

        



};








#endif //PROCESADORENLACES_h