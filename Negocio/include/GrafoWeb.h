#ifndef GRAFOWEB_H
#define GRAFOWEB_H

#include <string>
#include <vector>
#include <unordered_map>

/**
 * @brief Representa un grafo dirigido de enlaces web.
 * 
 * Cada nodo es una URL y cada arista representa un enlace
 * desde una página hacia otra.
 */
class GrafoWeb {
private:
    std::unordered_map<std::string, std::vector<std::string>> adyacencias;  

public:
    // Agrega un nodo al grafo si no existe
    void agregarNodo(const std::string& url);

    // Agrega una arista dirigida origen -> destino
    void agregarArista(const std::string& origen, const std::string& destino);

    // Verifica si un nodo existe
    bool existeNodo(const std::string& url) const;

    // Getter para obtener referencia constante al grafo interno
    const std::unordered_map<std::string, std::vector<std::string>>& getGrafo() const;

    // método size para comodidad
    size_t size() const { return adyacencias.size(); }

    // limpiar el grafo
    void clear() { adyacencias.clear(); }
};

#endif // GRAFOWEB_H