#include "Presentacion/include/MenuConsola.h"

int main() {
    // Fuerza UTF-8 en la consola de Windows (funciona en Windows 10 1903+ y Windows 11)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // para flechas y bordes
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    
    MenuConsola menu;
    menu.ejecutar();
    return 0;
}
