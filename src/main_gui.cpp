#include <windows.h>
#include "gui/MainWindow.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    Gui::MainWindow win(hInstance);

    if (!win.Create(500, 400, L"Offline Photo Enhancer")) {
        return 0;
    }

    return win.Run();
}
