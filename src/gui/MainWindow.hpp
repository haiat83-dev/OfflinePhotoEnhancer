#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <memory>
#include "../core/Engine.hpp"

namespace Gui {

    class MainWindow {
    public:
        MainWindow(HINSTANCE hInstance);
        ~MainWindow();

        bool Create(int width, int height, const std::wstring& title);
        int Run();

    private:
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

        void CreateControls();
        void OnDropFiles(HDROP hDrop);
        void OnStartClicked();
        void UpdateProgress(const Core::ProgressEvent& evt);

        HINSTANCE hInstance_;
        HWND hwnd_;
        
        // Controls
        HWND hBtnStart_;
        HWND hProgressBar_;
        HWND hStatusLabel_;
        HWND hEditInput_;
        HWND hComboScale_;
        HWND hComboModel_;

        // Engine
        std::unique_ptr<Core::Engine> engine_;
        std::vector<std::wstring> pendingFiles_;
        bool isProcessing_;
    };

}
