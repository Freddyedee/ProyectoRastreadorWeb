#ifndef PROCESADORENLACES_H
#define PROCESADORENLACES_H

#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>

class ProcesadorEnlaces {

    public: 

        ProcesadorEnlaces() = default; 

        explicit ProcesadorEnlaces(const std::string& dominio); 

        void setDominioBase(const std::string& dominio);

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
        
         /**
        * @brief 
        * Extrae el dominio principal de una url
        * 
        * @param url URL completa 
        * @return std::string (Dominio extraído por ejemplo: "ejemplo.com")
        */
        std::string extraerDominio(const std::string& url);

        std::string getDominioBase(){
            return this->dominioBase; 
        }; 
        
    private: 

       
        std::string dominioBase;
        std::string normalizarDominio(const std::string& dominio);

          // === Métodos privados para filtros internos ===
        bool esEnlaceValido(const std::string& url);      ///< Verifica si el enlace es navegable y no es vacío ni javascript/mail/tel
        bool esRecursoEstatico(const std::string& url);   ///< Filtra recursos estáticos como CSS, JS, imágenes
        bool perteneceAlDominio(const std::string& url);  ///< Comprueba si el enlace pertenece al dominio base
};

        











#endif //PROCESADORENLACES_h