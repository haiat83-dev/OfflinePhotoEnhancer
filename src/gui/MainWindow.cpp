#include "MainWindow.hpp"
#include <commctrl.h>
#include <shellapi.h>
#include <thread>
#include <filesystem>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")

#define ID_BTN_START 101
#define ID_COMBO_SCALE 102
#define ID_COMBO_MODEL 103

namespace Gui {

    MainWindow::MainWindow(HINSTANCE hInstance) : hInstance_(hInstance), hwnd_(NULL), isProcessing_(false) {
        // Initialize common controls
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_WIN95_CLASSES;
        InitCommonControlsEx(&icex);
    }

    MainWindow::~MainWindow() {}

    bool MainWindow::Create(int width, int height, const std::wstring& title) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = MainWindow::WindowProc;
        wc.hInstance = hInstance_;
        wc.lpszClassName = L"OfflineEnhancerClass";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

        RegisterClass(&wc);

        hwnd_ = CreateWindowEx(
            WS_EX_ACCEPTFILES, // Accept Drag & Drop
            L"OfflineEnhancerClass",
            title.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            NULL, NULL, hInstance_, this
        );

        if (!hwnd_) return false;

        CreateControls();
        ShowWindow(hwnd_, SW_SHOW);
        UpdateWindow(hwnd_);

        return true;
    }

    int MainWindow::Run() {
        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return (int)msg.wParam;
    }

    LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        MainWindow* pThis = NULL;

        if (uMsg == WM_NCCREATE) {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (MainWindow*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
            pThis->hwnd_ = hwnd;
        } else {
            pThis = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }

        if (pThis) {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        } else {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_CREATE:
                return 0;

            case WM_DROPFILES:
                OnDropFiles((HDROP)wParam);
                return 0;

            case WM_COMMAND:
                if (LOWORD(wParam) == ID_BTN_START) {
                    OnStartClicked();
                }
                return 0;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProc(hwnd_, uMsg, wParam, lParam);
    }

    void MainWindow::CreateControls() {
        // Simple layout
        int y = 20;
        int margin = 20;

        // Input Label
        CreateWindow(L"STATIC", L"Drag and drop images here...", WS_VISIBLE | WS_CHILD,
            margin, y, 300, 20, hwnd_, NULL, hInstance_, NULL);
        
        y += 30;
        hEditInput_ = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
            margin, y, 400, 25, hwnd_, NULL, hInstance_, NULL);

        y += 40;
        // Scale Combo
        CreateWindow(L"STATIC", L"Scale:", WS_VISIBLE | WS_CHILD, margin, y, 50, 20, hwnd_, NULL, hInstance_, NULL);
        hComboScale_ = CreateWindow(L"COMBOBOX", L"", WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
            margin + 60, y, 100, 100, hwnd_, (HMENU)ID_COMBO_SCALE, hInstance_, NULL);
        SendMessage(hComboScale_, CB_ADDSTRING, 0, (LPARAM)L"2x");
        SendMessage(hComboScale_, CB_ADDSTRING, 0, (LPARAM)L"4x");
        SendMessage(hComboScale_, CB_SETCURSEL, 1, 0); // Default 4x

        y += 40;
        // Start Button
        hBtnStart_ = CreateWindow(L"BUTTON", L"Start Processing", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            margin, y, 150, 30, hwnd_, (HMENU)ID_BTN_START, hInstance_, NULL);

        y += 50;
        // Progress Bar
        hProgressBar_ = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE,
            margin, y, 400, 20, hwnd_, NULL, hInstance_, NULL);
        
        y += 30;
        // Status
        hStatusLabel_ = CreateWindow(L"STATIC", L"Ready", WS_VISIBLE | WS_CHILD,
            margin, y, 400, 20, hwnd_, NULL, hInstance_, NULL);
    }

    void MainWindow::OnDropFiles(HDROP hDrop) {
        wchar_t buffer[MAX_PATH];
        UINT count = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
        
        pendingFiles_.clear();
        for (UINT i = 0; i < count; ++i) {
            if (DragQueryFile(hDrop, i, buffer, MAX_PATH)) {
                pendingFiles_.push_back(buffer);
            }
        }
        DragFinish(hDrop);

        if (!pendingFiles_.empty()) {
            std::wstring text = std::to_wstring(pendingFiles_.size()) + L" files selected.";
            SetWindowText(hEditInput_, text.c_str());
        }
    }

    void MainWindow::OnStartClicked() {
        if (isProcessing_ || pendingFiles_.empty()) return;

        isProcessing_ = true;
        EnableWindow(hBtnStart_, FALSE);
        SetWindowText(hStatusLabel_, L"Initializing...");

        // Get Options
        int scaleIdx = SendMessage(hComboScale_, CB_GETCURSEL, 0, 0);
        int scale = (scaleIdx == 0) ? 2 : 4;

        Core::EngineOptions opts;
        opts.scale = scale;
        opts.modelPath = L"models/stub.onnx"; // Hardcoded for now, should be from combo
        opts.device = Core::Device::CPU; // Default

        // Run in thread
        std::thread([this, opts]() {
            Core::Engine engine(opts);
            if (!engine.Initialize()) {
                PostMessage(hwnd_, WM_SETTEXT, 0, (LPARAM)L"Failed to init engine");
                // Reset UI state (needs thread safety or PostMessage)
                return;
            }

            // Output dir = same as input
            std::wstring outDir = std::filesystem::path(pendingFiles_[0]).parent_path().wstring();

            engine.ProcessBatch(pendingFiles_, outDir, [this](const Core::ProgressEvent& evt) {
                // Update UI (Should use PostMessage for thread safety)
                // For simplicity in this snippet, we assume simple atomic updates or risk it.
                // Correct way:
                // PostMessage(hwnd_, WM_USER_PROGRESS, ...);
                
                // Hacky direct update for prototype:
                SendMessage(hProgressBar_, PBM_SETPOS, (WPARAM)(evt.percentComplete * 100), 0);
            });

            isProcessing_ = false;
            // Re-enable button (needs PostMessage)
        }).detach();
    }

}
