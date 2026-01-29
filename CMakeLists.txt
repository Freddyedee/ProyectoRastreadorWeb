# =============================================================================
#  CMakeLists.txt — Proyecto con libcurl (Arquitectura por Capas)
# =============================================================================
#  Este archivo configura la compilación del proyecto utilizando CMake.
#  Integra:
#   - Organización por capas (Datos / Negocio / Presentación)
#   - Verificación automática de archivos .cpp
#   - Integración con libcurl vía vcpkg
#
#  Estructura esperada:
#      src/
#          Datos/
#          Negocio/
#          Presentacion/
#      include/
# =============================================================================


cmake_minimum_required(VERSION 3.15)
project(MyCurlProject VERSION 1.0 LANGUAGES CXX)


# =============================================================================
# CONFIGURACIÓN DE VCPKG Y LIBCURL
# =============================================================================
# Permite que CMake encuentre libcurl automáticamente

set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_package(CURL REQUIRED)


# =============================================================================
# CONFIGURACIÓN DEL COMPILADOR Y ESTÁNDAR C++
# =============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opcional: enlazado estático (útil en Windows)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")


# =============================================================================
# GESTIÓN DE ARCHIVOS FUENTE
# =============================================================================

message(STATUS "=== VERIFICACIÓN DE ARCHIVOS FUENTE ===")

set(SOURCE_FILES "")


# -----------------------------------------------------------------------------
# LISTA CENTRALIZADA DE ARCHIVOS .cpp
#
# INSTRUCCIONES:
# 1. Crear el archivo en src/Datos, src/Negocio o src/Presentacion
# 2. Agregar la ruta exacta aquí
# 3. CMake verificará automáticamente su existencia
# -----------------------------------------------------------------------------

set(FILES_TO_CHECK
    # Archivo principal

    # ========================
    # MÓDULO: Datos
    # ========================
    # "src/Datos/RepositorioDatos.cpp"

    # ========================
    # MÓDULO: Negocio
    # ========================
    "Rastreador.cpp"
    "Negocio/src/ComunicacionHTTP.cpp"
    "Negocio/src/GrafoWeb.cpp"
    "Negocio/src/ProcesadorEnlaces.cpp"
    "Negocio/src/WebCrawler.cpp"
    "Negocio/src/BuscadorClave.cpp"
    "Negocio/src/AnalizadorGrafo.cpp"
    "Datos/src/GestorArchivos.cpp"
    "Presentacion/src/MenuConsola.cpp"


    # ========================
    # MÓDULO: Presentación
    # ========================
   
)


# =============================================================================
# VERIFICACIÓN AUTOMÁTICA DE ARCHIVOS
# =============================================================================

foreach(file ${FILES_TO_CHECK})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        message(STATUS "✓ ENCONTRADO: ${file}")
        list(APPEND SOURCE_FILES ${file})
    else()
        message(WARNING "⚠️  NO ENCONTRADO: ${file}")
        message(STATUS  "   Verifica:")
        message(STATUS  "   - Nombre exacto del archivo")
        message(STATUS  "   - Ubicación dentro de src/")
        message(STATUS  "   - Extensión .cpp")
    endif()
endforeach()

message(STATUS "Total de archivos a compilar: ${SOURCE_FILES}")
message(STATUS "==============================================")


# =============================================================================
# CONFIGURACIÓN DEL EJECUTABLE
# =============================================================================

add_executable(myapp ${SOURCE_FILES})


# =============================================================================
# INCLUSIÓN DE HEADERS
# =============================================================================
# Todos los .h viven en include/ y subcarpetas

target_include_directories(myapp PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/Negocio/include
)


# =============================================================================
# ENLAZADO DE LIBRERÍAS
# =============================================================================

target_link_libraries(myapp PRIVATE CURL::libcurl)


# =============================================================================
# PROPIEDADES DEL EJECUTABLE
# =============================================================================

set_target_properties(myapp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    OUTPUT_NAME "Rastreador"
)


# =============================================================================
# OPCIONES DE COMPILACIÓN SEGÚN MODO
# =============================================================================

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(myapp PRIVATE -Wall -g -O0)
    message(STATUS "Modo: Debug — depuración activa")
else()
    target_compile_options(myapp PRIVATE -O2)
    message(STATUS "Modo: Release — optimización activada")
endif()


# =============================================================================
# MENSAJE FINAL
# =============================================================================

message(STATUS "-> Configuración del proyecto completada.")
message(STATUS "-> Ejecutable generado en: bin/MyCurlApp.exe")
message(STATUS "-> Para agregar nuevos archivos, edita FILES_TO_CHECK.")
