#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <chrono>
#include <math.h>
#include <algorithm>
#include <random>
#include <shellapi.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "Shell32.lib")

using namespace Gdiplus;
using namespace std;

// --- DATA STRUCTURES ---

struct Theme {
    wstring name;
    Color bg;
    Color text;
    Color dim;
    Color accent;
    Color grad_s;
    Color grad_e;
};

// --- APP STATE ---

class StopwatchApp {
public:
    HWND hwnd = NULL;
    bool running = false;
    double elapsedSec = 0;
    chrono::steady_clock::time_point lastUpdate;
    
    bool isTimer = false;
    int themeIndex = 5; // Default: Forest
    vector<Theme> themes;
    vector<Color> palette;
    int rotationIndex = 0;
    
    bool isHovered = false;
    int intensityLevel = 0; 
    
    // Pet & Motivation
    int petBounce = 0;
    vector<wstring> motivations;
    int currentMotivationIdx = 0;
    int motivationTimer = 0; 
    bool motivationActive = false;
    int lastMinute = 0;
    
    // Productivity Mode
    bool productivityMode = false;
    int wiggleTimer = 0;
    int wiggleFrames = 0;
    int lastWiggleMinute = 0;

    // Compact Mode
    bool isCompact = false;
    int normalW = 210, normalH = 70;
    int compactW = 120, compactH = 40;
    
    StopwatchApp() {
        themes = {
            { L"Windows 11", Color(255, 255, 255, 255), Color(255, 0, 0, 0), Color(255, 136, 136, 136), Color(255, 28, 110, 164), Color(255, 216, 239, 171), Color(255, 52, 152, 219) },
            { L"Cyberpunk", Color(255, 13, 13, 13), Color(255, 0, 255, 65), Color(255, 0, 143, 17), Color(255, 0, 255, 65), Color(255, 255, 0, 255), Color(255, 0, 255, 255) },
            { L"Midnight", Color(255, 26, 26, 46), Color(255, 233, 69, 96), Color(255, 78, 78, 106), Color(255, 15, 52, 96), Color(255, 233, 69, 96), Color(255, 22, 33, 62) },
            { L"Rose Gold", Color(255, 255, 245, 245), Color(255, 183, 110, 121), Color(255, 212, 165, 165), Color(255, 229, 179, 187), Color(255, 255, 192, 203), Color(255, 183, 110, 121) },
            { L"Nord", Color(255, 46, 52, 64), Color(255, 236, 239, 244), Color(255, 76, 86, 106), Color(255, 136, 192, 208), Color(255, 129, 161, 193), Color(255, 136, 192, 208) },
            { L"Forest", Color(255, 27, 46, 27), Color(255, 220, 252, 231), Color(255, 63, 98, 18), Color(255, 34, 197, 94), Color(255, 22, 101, 52), Color(255, 134, 239, 172) },
            { L"Dracula", Color(255, 40, 42, 54), Color(255, 248, 248, 242), Color(255, 98, 114, 164), Color(255, 189, 147, 249), Color(255, 255, 121, 198), Color(255, 189, 147, 249) },
            { L"Sunset", Color(255, 45, 27, 45), Color(255, 255, 158, 125), Color(255, 99, 75, 99), Color(255, 255, 95, 109), Color(255, 255, 95, 109), Color(255, 255, 195, 113) }
        };
        
        motivations = {
            L"KEEP PUSHING!", L"YOU'VE GOT THIS!", L"STAY FOCUSED!", L"ALMOST THERE!",
            L"CONSISTENCY IS KEY!", L"ONE STEP AT A TIME!", L"PROGRESS IS PROGRESS!",
            L"STAY HUNGRY!", L"DON'T STOP NOW!", L"KEEP THE MOMENTUM!", L"YOU ARE UNSTOPPABLE!",
            L"MAKE IT COUNT!", L"PROVE THEM WRONG!", L"FOCUS ON THE GOAL!", L"DO IT FOR YOU!"
        };
        
        UpdatePalette();
    }

    void UpdatePalette() {
        Theme& t = themes[themeIndex];
        GeneratePalette(t.grad_s, t.grad_e, 45);
    }

    void GeneratePalette(Color c1, Color c2, int n) {
        palette.clear();
        for (int i = 0; i < n; i++) {
            float t = (float)i / (n - 1);
            BYTE r = (BYTE)(c1.GetR() + (c2.GetR() - c1.GetR()) * t);
            BYTE g = (BYTE)(c1.GetG() + (c2.GetG() - c1.GetG()) * t);
            BYTE b = (BYTE)(c1.GetB() + (c2.GetB() - c1.GetB()) * t);
            palette.push_back(Color(255, r, g, b));
        }
        for (int i = n - 1; i >= 0; i--) { palette.push_back(palette[i]); }
    }

    void UpdateIntensity() {
        double mins = abs(elapsedSec) / 60.0;
        int newLevel = 0;
        if (mins >= 50) newLevel = 2;
        else if (mins >= 25) newLevel = 1;
        if (newLevel != intensityLevel) {
            intensityLevel = newLevel;
            if (newLevel == 1) GeneratePalette(Color(255, 241, 196, 15), Color(255, 39, 174, 96), 45);
            else if (newLevel == 2) GeneratePalette(Color(255, 230, 126, 34), Color(255, 231, 76, 60), 45);
            else UpdatePalette();
        }
    }

    wstring GetTimeString() {
        double sTotal = abs(elapsedSec);
        int h = (int)(sTotal / 3600), m = (int)((sTotal - h * 3600) / 60), s = (int)(sTotal - h * 3600 - m * 60);
        wchar_t b[64]; swprintf(b, L"%ls%02d:%02d:%02d", (elapsedSec < 0 ? L"-" : L""), h, m, s);
        return b;
    }
    
    wstring GetMsString() {
        int ms = (int)((abs(elapsedSec) - floor(abs(elapsedSec))) * 100);
        wchar_t b[16]; swprintf(b, L".%02d", ms);
        return b;
    }

    void TriggerMotivation() { motivationActive = true; motivationTimer = 0; currentMotivationIdx = rand() % motivations.size(); }

    bool IsFullscreen() {
        HWND fg = GetForegroundWindow();
        if (!fg || fg == GetDesktopWindow() || fg == GetShellWindow()) return false;
        RECT rc; GetWindowRect(fg, &rc);
        return (rc.left <= 0 && rc.top <= 0 && rc.right >= GetSystemMetrics(SM_CXSCREEN) && rc.bottom >= GetSystemMetrics(SM_CYSCREEN));
    }

    void ToggleCompact() {
        isCompact = !isCompact;
        int w = isCompact ? compactW : normalW;
        int h = isCompact ? compactH : normalH;
        SetWindowPos(hwnd, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
};

StopwatchApp app;

// --- INPUT DIALOG ---

struct InputParams { wstring result; bool submitted = false; };
LRESULT CALLBACK InputWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static HWND hEdit; static InputParams* params;
    switch (msg) {
    case WM_CREATE:
        params = (InputParams*)((LPCREATESTRUCT)lp)->lpCreateParams;
        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"20", WS_CHILD | WS_VISIBLE | ES_NUMBER, 10, 10, 100, 25, hwnd, NULL, NULL, NULL);
        CreateWindow(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 120, 10, 50, 25, hwnd, (HMENU)1, NULL, NULL);
        SetFocus(hEdit); return 0;
    case WM_COMMAND: if (LOWORD(wp) == 1) { wchar_t b[32]; GetWindowText(hEdit, b, 32); params->result = b; params->submitted = true; DestroyWindow(hwnd); } return 0;
    case WM_CLOSE: DestroyWindow(hwnd); return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}
double GetTimerInput(HWND parent) {
    InputParams params; WNDCLASS wc = { 0 }; wc.lpfnWndProc = InputWndProc; wc.hInstance = GetModuleHandle(NULL); wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); wc.lpszClassName = L"InputBox"; RegisterClass(&wc);
    HWND hDlg = CreateWindowEx(WS_EX_TOPMOST, L"InputBox", L"Set Timer (min)", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, 200, 200, 200, 85, parent, NULL, NULL, &params);
    ShowWindow(hDlg, SW_SHOW); UpdateWindow(hDlg);
    MSG msg; while (IsWindow(hDlg) && GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    if (params.submitted) return _wtof(params.result.c_str()); return 0;
}

// --- DRAWING ENGINE ---

void Render(HWND hwnd) {
    RECT rc; GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left, height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0) return;

    HDC hdcScreen = GetDC(hwnd); HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdcScreen, width, height); SelectObject(hdcMem, hbmMem);
    Graphics g(hdcMem); g.SetSmoothingMode(SmoothingModeAntiAlias); g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

    Theme& theme = app.themes[app.themeIndex];
    int r = app.isCompact ? 8 : 12;

    g.Clear(Color(0, 0, 0, 0));
    GraphicsPath path;
    path.AddArc(2, 2, r * 2, r * 2, 180, 90);
    path.AddArc(width - r * 2 - 2, 2, r * 2, r * 2, 270, 90);
    path.AddArc(width - r * 2 - 2, height - r * 2 - 2, r * 2, r * 2, 0, 90);
    path.AddArc(2, height - r * 2 - 2, r * 2, r * 2, 90, 90);
    path.CloseFigure();

    SolidBrush bgBrush(theme.bg); g.FillPath(&bgBrush, &path);
    if (app.running) app.rotationIndex = (app.rotationIndex + 1) % app.palette.size();
    Color borderColor = app.palette[app.rotationIndex % app.palette.size()];
    if (app.isTimer && app.elapsedSec < 0 && (app.rotationIndex / 5) % 2 == 0) borderColor = Color(255, 255, 0, 0);
    Pen borderPen(borderColor, 2.5f); g.DrawPath(&borderPen, &path);

    if (app.isHovered) { SolidBrush dimBrush(Color(200, 0, 0, 0)); g.FillPath(&dimBrush, &path); }

    FontFamily ff(L"Segoe UI");
    Font fontMain(&ff, (REAL)(app.isCompact ? 18 : 26), FontStyleBold, UnitPixel);
    Font fontSmall(&ff, (REAL)(app.isCompact ? 10 : 12), FontStyleRegular, UnitPixel);
    Font fontIcons(L"Segoe MDL2 Assets", (REAL)(app.isCompact ? 10 : 14), FontStyleRegular, UnitPixel);

    SolidBrush textBrush(theme.text); SolidBrush dimTextBrush(theme.dim); SolidBrush accentBrush(theme.accent);

    if (!app.isHovered) {
        wstring t = app.GetTimeString(), ms = app.GetMsString();
        Color activeTextColor = theme.text;
        if (app.isTimer && app.elapsedSec < 0 && (app.rotationIndex / 10) % 2 == 0) activeTextColor = Color(255, 255, 0, 0);
        SolidBrush activeTextBrush(activeTextColor);

        RectF bounds; g.MeasureString(t.c_str(), -1, &fontMain, PointF(0, 0), &bounds);
        float tx = (width - bounds.Width) / 2 - (app.isCompact ? 2 : 5);
        float ty = (height - bounds.Height) / 2 - (app.isCompact ? 2 : 5);
        g.DrawString(t.c_str(), -1, &fontMain, PointF(tx, ty), &activeTextBrush);
        if (!app.isCompact) g.DrawString(ms.c_str(), -1, &fontSmall, PointF(tx + bounds.Width - 2, ty + 12), &accentBrush);
        
        if (!app.isCompact) {
            // Pet & Motivation
            wstring pet = (app.running ? L"\U0001F63A" : L"\U0001F634");
            if (app.intensityLevel == 1) pet = L"\U0001F640"; else if (app.intensityLevel == 2) pet = L"\U0001F525";
            float petX = 12, petY = height - 28;
            if (app.running) { app.petBounce = (app.petBounce + 1) % 10; if (app.petBounce > 5) petY -= 2; }
            SolidBrush petBgBrush(Color(60, theme.accent.GetR(), theme.accent.GetG(), theme.accent.GetB()));
            g.FillEllipse(&petBgBrush, (REAL)(petX - 2), (REAL)(petY + 2), 24.0f, 24.0f);
            Font fontEmoji(L"Segoe UI Emoji", 18, FontStyleRegular, UnitPixel);
            g.DrawString(pet.c_str(), -1, &fontEmoji, PointF(petX, petY), &textBrush);
            
            if (app.motivationActive) {
                app.motivationTimer++;
                if (app.motivationTimer < 200) {
                    int cIdx = (app.motivationTimer / 10) % 7; Color rain[] = { Color(255,255,0,0), Color(255,255,127,0), Color(255,255,255,0), Color(255,0,255,0), Color(255,0,0,255), Color(255,75,0,130), Color(255,148,0,211) };
                    SolidBrush mBr(rain[cIdx]); Font fontMotiv(&ff, 10, FontStyleBold, UnitPixel);
                    StringFormat mF; mF.SetAlignment(StringAlignmentCenter);
                    g.DrawString(app.motivations[app.currentMotivationIdx].c_str(), -1, &fontMotiv, RectF(35, height - 25, (float)width - 45, 20), &mF, &mBr);
                } else app.motivationActive = false;
            }
        }
    } else {
        // Controls (8 Buttons)
        StringFormat cf; cf.SetAlignment(StringAlignmentCenter); cf.SetLineAlignment(StringAlignmentCenter);
        float bw = app.isCompact ? 14 : 24, bh = app.isCompact ? 14 : 24, gap = 1;
        float startX = (width - (8 * bw + 7 * gap)) / 2; float y = (height - bh) / 2;
        wstring icons[] = { (app.running ? L"\uE769" : L"\uE768"), (app.isTimer ? L"\uE916" : L"\uE706"), L"\uE710", L"\uE771", L"\uE707", L"\uE928", L"\uE712", L"\uE711" };
        Color iconColors[] = { theme.text, theme.text, Color(255, 39, 174, 96), theme.text, theme.accent, (app.productivityMode ? Color(255, 255, 215, 0) : theme.dim), (app.isCompact ? Color(255, 0, 120, 215) : theme.text), Color(255, 196, 43, 28) };
        for (int i = 0; i < 8; i++) {
            RectF br(startX + i * (bw + gap), y, bw, bh); SolidBrush bBg(Color(100, 100, 100, 100)); g.FillRectangle(&bBg, br);
            SolidBrush iBr(iconColors[i]); g.DrawString(icons[i].c_str(), -1, &fontIcons, br, &cf, &iBr);
        }
    }

    POINT ptSrc = { 0, 0 }; SIZE sizeWnd = { width, height }; BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    POINT ptDest; RECT wrc; GetWindowRect(hwnd, &wrc); ptDest.x = wrc.left; ptDest.y = wrc.top;
    if (app.wiggleFrames > 0) { app.wiggleFrames--; ptDest.y += (int)(sin(app.wiggleFrames * 0.5) * 10); }
    UpdateLayeredWindow(hwnd, hdcScreen, &ptDest, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    DeleteObject(hbmMem); DeleteDC(hdcMem); ReleaseDC(hwnd, hdcScreen);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: srand((unsigned)time(NULL)); SetTimer(hwnd, 1, 30, NULL); return 0;
    case WM_TIMER:
        if (app.running) {
            auto now = chrono::steady_clock::now(); double d = chrono::duration<double>(now - app.lastUpdate).count();
            if (app.isTimer) app.elapsedSec -= d; else app.elapsedSec += d;
            app.lastUpdate = now; app.UpdateIntensity();
            int curM = (int)(abs(app.elapsedSec) / 60);
            if (curM != app.lastMinute) {
                app.lastMinute = curM; app.TriggerMotivation();
                if (curM % 5 == 0 && app.productivityMode) app.wiggleFrames = 60;
                Beep(800, 50);
            }
        }
        if (app.productivityMode && app.IsFullscreen()) {
            RECT wr; GetWindowRect(hwnd, &wr); int sw = GetSystemMetrics(SM_CXSCREEN);
            int tx = ((wr.left + (wr.right - wr.left) / 2) < sw / 2) ? 20 : sw - (wr.right - wr.left) - 20;
            if (abs(wr.left - tx) > 5 || wr.top > 20) SetWindowPos(hwnd, HWND_TOPMOST, tx, 20, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
        }
        Render(hwnd); return 0;
    case WM_LBUTTONDOWN: {
        int x = LOWORD(lp), y = HIWORD(lp); RECT rc; GetClientRect(hwnd, &rc); int w = rc.right - rc.left, h = rc.bottom - rc.top;
        if (app.isHovered) {
            float bw = app.isCompact ? 14 : 24, bh = app.isCompact ? 14 : 24, gap = 1;
            float startX = (w - (8 * bw + 7 * gap)) / 2; float bY = (h - bh) / 2;
            if (y >= bY && y <= bY + bh) {
                int bIdx = -1; for (int i = 0; i < 8; i++) { float bx = startX + i * (bw + gap); if (x >= bx && x <= bx + bw) { bIdx = i; break; } }
                if (bIdx == 0) { if (app.running) app.running = false; else { app.running = true; app.lastUpdate = chrono::steady_clock::now(); } }
                else if (bIdx == 1) { if (!app.running) { app.isTimer = !app.isTimer; if (app.isTimer) app.elapsedSec = GetTimerInput(hwnd) * 60.0; else app.elapsedSec = 0; } }
                else if (bIdx == 2) { wchar_t p[MAX_PATH]; GetModuleFileName(NULL, p, MAX_PATH); ShellExecute(NULL, L"open", p, NULL, NULL, SW_SHOW); }
                else if (bIdx == 3) { app.themeIndex = (app.themeIndex + 1) % app.themes.size(); app.UpdatePalette(); }
                else if (bIdx == 4) { app.themeIndex = rand() % app.themes.size(); app.UpdatePalette(); }
                else if (bIdx == 5) app.productivityMode = !app.productivityMode;
                else if (bIdx == 6) { app.ToggleCompact(); Beep(1000, 50); }
                else if (bIdx == 7) PostQuitMessage(0);
                return 0;
            }
        }
        SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0); return 0;
    }
    case WM_MOUSEMOVE: { TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 }; TrackMouseEvent(&tme); if (!app.isHovered) { app.isHovered = true; Render(hwnd); } return 0; }
    case WM_MOUSELEAVE: app.isHovered = false; Render(hwnd); return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int) {
    GdiplusStartupInput gsi; ULONG_PTR gst; GdiplusStartup(&gst, &gsi, NULL);
    WNDCLASS wc = { 0 }; wc.lpfnWndProc = WndProc; wc.hInstance = hI; wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.lpszClassName = L"StopwatchProCPP"; RegisterClass(&wc);
    app.hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW, wc.lpszClassName, L"Stopwatch Pro", WS_POPUP | WS_VISIBLE, 
                               GetSystemMetrics(SM_CXSCREEN) - 210 - 40, 40, 210, 70, NULL, NULL, hI, NULL);
    MSG msg; while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    GdiplusShutdown(gst); return 0;
}
