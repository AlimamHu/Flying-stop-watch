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
    wstring name; Color bg; Color text; Color dim; Color accent; Color grad_s; Color grad_e;
};

// --- APP STATE ---

class StopwatchApp {
public:
    HWND hwnd = NULL;
    bool running = false;
    double elapsedSec = 0;
    chrono::steady_clock::time_point lastUpdate;
    
    bool isTimer = false;
    int themeIndex = 5; 
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
    int wiggleFrames = 0;

    // Compact Mode (Multi-level)
    int compactLevel = 0; 
    bool isLiquidMode = false;
    wstring taskName = L"CURRENT TASK";
    bool showTask = true;

    // Pomodoro Mode
    bool isPomodoro = false;
    bool isRestPhase = false;
    double workTimeSec = 0;
    double restTimeSec = 0;
    vector<wstring> restMessages;

    struct ModeConfig { int w, h; float mainF, smallF; bool showPet, showMs; };
    vector<ModeConfig> modes = {
        { 210, 70, 26.0f, 12.0f, true, true },
        { 165, 58, 20.0f, 10.0f, true, true },
        { 130, 48, 16.0f, 9.0f, false, true },
        { 95, 38, 13.0f, 0.0f, false, true },
        { 80, 30, 11.0f, 0.0f, false, true } // 4: Super Nano
    };
    
    double initialTimerSec = 0;
    
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
            L"KEEP PUSHING!", L"YOU'VE GOT THIS!", L"STAY FOCUSED!", L"ALMOST THERE!", L"CONSISTENCY IS KEY!", 
            L"ONE STEP AT A TIME!", L"PROGRESS IS PROGRESS!", L"STAY HUNGRY!", L"DON'T STOP NOW!", 
            L"KEEP THE MOMENTUM!", L"YOU ARE UNSTOPPABLE!", L"MAKE IT COUNT!", L"PROVE THEM WRONG!", 
            L"FOCUS ON THE GOAL!", L"DO IT FOR YOU!"
        };
        restMessages = { 
            L"chessss simple", L"Sudoku", L"Memory Game", L"Increase your photography memory", 
            L"play go", L"take deep breath", L"up and down eye ball", L"walk" 
        };
        UpdatePalette();
    }

    void UpdatePalette() { Theme& t = themes[themeIndex]; GeneratePalette(t.grad_s, t.grad_e, 45); }
    void GeneratePalette(Color c1, Color c2, int n) {
        palette.clear();
        for (int i = 0; i < n; i++) {
            float t = (float)i / (n - 1);
            BYTE r = (BYTE)(c1.GetR() + (c2.GetR() - c1.GetR()) * t);
            BYTE g = (BYTE)(c1.GetG() + (c2.GetG() - c1.GetG()) * t);
            BYTE b = (BYTE)(c1.GetB() + (c2.GetB() - c1.GetB()) * t);
            palette.push_back(Color(255, r, g, b));
        }
        for (int i = n - 1; i >= 0; i--) palette.push_back(palette[i]);
    }

    void UpdateIntensity() {
        int newLevel = 0;
        if (isTimer) {
            // TIMER MODE: Urgency increases as time decreases
            if (elapsedSec < 300) newLevel = 2;      // < 5 mins: INTENSE (Red)
            else if (elapsedSec < 900) newLevel = 1; // < 15 mins: FOCUSED (Yellow)
            else newLevel = 0;
        } else {
            // STOPWATCH MODE: Intensity increases as time increases
            double mins = elapsedSec / 60.0;
            if (mins >= 50) newLevel = 2; else if (mins >= 25) newLevel = 1;
        }

        if (newLevel != intensityLevel) {
            intensityLevel = newLevel;
            if (newLevel == 1) GeneratePalette(Color(255, 241, 196, 15), Color(255, 39, 174, 96), 45);
            else if (newLevel == 2) GeneratePalette(Color(255, 230, 126, 34), Color(255, 231, 76, 60), 45);
            else UpdatePalette();
        }
    }

    wstring GetTimeString() {
        double sT = abs(elapsedSec); int h = (int)(sT / 3600), m = (int)((sT - h * 3600) / 60), s = (int)(sT - h * 3600 - m * 60);
        wchar_t b[64]; swprintf(b, L"%ls%02d:%02d:%02d", (elapsedSec < 0 ? L"-" : L""), h, m, s); return b;
    }
    wstring GetMsString() { int ms = (int)((abs(elapsedSec) - floor(abs(elapsedSec))) * 100); wchar_t b[16]; swprintf(b, L".%02d", ms); return b; }
    void TriggerMotivation() { 
        motivationActive = true; 
        motivationTimer = 0; 
        if (isRestPhase) {
            currentMotivationIdx = rand() % restMessages.size();
        } else {
            currentMotivationIdx = rand() % motivations.size();
        }
    }

    bool IsFullscreen() {
        HWND fg = GetForegroundWindow(); if (!fg || fg == GetDesktopWindow() || fg == GetShellWindow()) return false;
        RECT rc; GetWindowRect(fg, &rc); return (rc.left <= 0 && rc.top <= 0 && rc.right >= GetSystemMetrics(SM_CXSCREEN) && rc.bottom >= GetSystemMetrics(SM_CYSCREEN));
    }

    void CycleCompactMode() {
        compactLevel = (compactLevel + 1) % modes.size();
        int w = modes[compactLevel].w, h = modes[compactLevel].h;
        SetWindowPos(hwnd, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
};

StopwatchApp app;

// --- INPUT DIALOG ---
struct InputParams { wstring result; bool submitted = false; bool numeric = false; };
LRESULT CALLBACK InputWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static HWND hEdit; static InputParams* params;
    switch (msg) {
    case WM_CREATE: params = (InputParams*)((LPCREATESTRUCT)lp)->lpCreateParams; hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", params->result.c_str(), WS_CHILD | WS_VISIBLE | (params->numeric ? ES_NUMBER : 0), 10, 10, 100, 25, hwnd, NULL, NULL, NULL); CreateWindow(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 120, 10, 50, 25, hwnd, (HMENU)1, NULL, NULL); SetFocus(hEdit); return 0;
    case WM_COMMAND: if (LOWORD(wp) == 1) { wchar_t b[32]; GetWindowText(hEdit, b, 32); params->result = b; params->submitted = true; DestroyWindow(hwnd); } return 0;
    case WM_CLOSE: DestroyWindow(hwnd); return 0; } return DefWindowProc(hwnd, msg, wp, lp);
}
wstring GetTaskInput(HWND parent) {
    InputParams p; p.result = app.taskName; p.numeric = false; WNDCLASS wc = { 0 }; wc.lpfnWndProc = InputWndProc; wc.hInstance = GetModuleHandle(NULL); wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); wc.lpszClassName = L"TaskInputBox"; RegisterClass(&wc);
    HWND hD = CreateWindowEx(WS_EX_TOPMOST, L"TaskInputBox", L"Enter Task Name", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, 200, 200, 250, 85, parent, NULL, NULL, &p); ShowWindow(hD, SW_SHOW); UpdateWindow(hD);
    MSG m; while (IsWindow(hD) && GetMessage(&m, NULL, 0, 0)) { TranslateMessage(&m); DispatchMessage(&m); } if (p.submitted) return p.result; return L"";
}

double GetTimerInput(HWND parent, wstring title = L"Set Timer (min)") {
    InputParams p; p.result = L"20"; p.numeric = true; WNDCLASS wc = { 0 }; wc.lpfnWndProc = InputWndProc; wc.hInstance = GetModuleHandle(NULL); wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); wc.lpszClassName = title.c_str(); RegisterClass(&wc);
    HWND hD = CreateWindowEx(WS_EX_TOPMOST, title.c_str(), title.c_str(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, 200, 200, 200, 85, parent, NULL, NULL, &p); ShowWindow(hD, SW_SHOW); UpdateWindow(hD);
    MSG m; while (IsWindow(hD) && GetMessage(&m, NULL, 0, 0)) { TranslateMessage(&m); DispatchMessage(&m); } if (p.submitted) return _wtof(p.result.c_str()); return 0;
}

// --- DRAWING ENGINE ---

void Render(HWND hwnd) {
    RECT rc; GetClientRect(hwnd, &rc); int width = rc.right - rc.left, height = rc.bottom - rc.top; if (width <= 0 || height <= 0) return;
    HDC hdcS = GetDC(hwnd); HDC hdcM = CreateCompatibleDC(hdcS); HBITMAP hbmM = CreateCompatibleBitmap(hdcS, width, height); SelectObject(hdcM, hbmM);
    Graphics g(hdcM); g.SetSmoothingMode(SmoothingModeAntiAlias); g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

    Theme& theme = app.themes[app.themeIndex]; 
    auto& cfg = app.modes[app.compactLevel];
    int r = (app.compactLevel > 0) ? 10 : 12;

    g.Clear(Color(0, 0, 0, 0));
    GraphicsPath path; path.AddArc(2, 2, r * 2, r * 2, 180, 90); path.AddArc(width - r * 2 - 2, 2, r * 2, r * 2, 270, 90); path.AddArc(width - r * 2 - 2, height - r * 2 - 2, r * 2, r * 2, 0, 90); path.AddArc(2, height - r * 2 - 2, r * 2, r * 2, 90, 90); path.CloseFigure();
    SolidBrush bgBr(theme.bg); g.FillPath(&bgBr, &path);
    if (app.running) app.rotationIndex = (app.rotationIndex + 1) % app.palette.size();
    Color bColor = app.palette[app.rotationIndex % app.palette.size()];
    if (app.isTimer && app.elapsedSec < 0 && (app.rotationIndex / 5) % 2 == 0) bColor = Color(255, 255, 0, 0);

    // Draw Border (Except in Super Nano mode)
    if (app.compactLevel != 4) {
        Pen bPen(bColor, 1.2f);
        g.DrawPath(&bPen, &path);
    }

    if (app.isHovered) { SolidBrush dBr(Color(200, 0, 0, 0)); g.FillPath(&dBr, &path); }

    FontFamily ff(L"Segoe UI");
    Font fMain(&ff, cfg.mainF, FontStyleBold, UnitPixel);
    float msSize = (cfg.smallF > 0) ? cfg.smallF : (app.compactLevel >= 3 ? 8.0f : 10.0f);
    Font fSmall(&ff, msSize, FontStyleRegular, UnitPixel);
    Font fIcons(L"Segoe MDL2 Assets", (app.compactLevel > 1 ? 11 : 13), FontStyleRegular, UnitPixel);
    SolidBrush tBr(theme.text); SolidBrush dBr(theme.dim); SolidBrush aBr(theme.accent);

    if (!app.isHovered) {
        wstring t = app.GetTimeString(), ms = app.GetMsString();
        Color aTColor = theme.text; if (app.isTimer && app.elapsedSec < 0 && (app.rotationIndex / 10) % 2 == 0) aTColor = Color(255, 255, 0, 0);
        
        // Super Nano Fade Logic
        BYTE textAlpha = 255;
        if (app.compactLevel == 4) {
            int cycle = app.rotationIndex % 100; // ~3 seconds cycle at 30ms timer
            if (cycle > 50) textAlpha = (BYTE)(255 * (1.0f - (cycle - 50) / 50.0f));
        }
        SolidBrush aTBr(Color(textAlpha, aTColor.GetR(), aTColor.GetG(), aTColor.GetB())); 
        RectF bnd; g.MeasureString(t.c_str(), -1, &fMain, PointF(0, 0), &bnd);
        
        // Text Positioning
        float tx = (width - bnd.Width) / 2 - (cfg.showMs ? (app.compactLevel >= 3 ? 8 : 12) : 0);
        float ty = (height - bnd.Height) / 2;
        // Shift up slightly ONLY if liquid bar is showing and takes space
        if ((app.isLiquidMode || app.compactLevel == 4) && app.isTimer && app.initialTimerSec > 0) {
            ty -= (app.compactLevel >= 2 ? 3 : 6);
        }
        
        g.DrawString(t.c_str(), -1, &fMain, PointF(tx, ty), &aTBr);
        
        if (cfg.showMs) {
            SolidBrush aBrFade(Color(textAlpha, theme.accent.GetR(), theme.accent.GetG(), theme.accent.GetB()));
            float msY = ty + (height > 50 ? 10 : (app.compactLevel >= 3 ? 4 : 6));
            g.DrawString(ms.c_str(), -1, &fSmall, PointF(tx + bnd.Width - 1, msY), &aBrFade);
        }
        
        if (app.showTask && app.compactLevel < 2) {
            g.DrawString(app.taskName.c_str(), -1, &fSmall, PointF(10, 6), &dBr);
        }

        // Compact Liquid Progress Bar (Premium Capsule Look)
        if ((app.compactLevel > 0 && app.isLiquidMode && app.isTimer && app.initialTimerSec > 0) || (app.compactLevel == 4 && app.isTimer && app.initialTimerSec > 0)) {
            float progress = (float)(app.elapsedSec / app.initialTimerSec);
            bool isOvertime = app.elapsedSec < 0;
            if (isOvertime) progress = (float)(fmod(abs(app.elapsedSec), 120.0) / 120.0); // Fill back up every 2 mins in overtime
            if (progress < 0) progress = 0; if (progress > 1) progress = 1;

            // 2-Minute Breathing Cycle
            float cycle = (float)(sin(app.elapsedSec * 3.14159 / 60.0) * 0.5 + 0.5); // Fill/Empty every 2 mins
            
            float barMaxW = (float)width - 20;
            float fillW = barMaxW * progress;
            float capH = (app.compactLevel >= 2) ? (app.compactLevel == 4 ? 6.0f : 10.0f) : 16.0f;
            float capY = (float)height - capH - (app.compactLevel >= 2 ? (app.compactLevel == 4 ? 4.0f : 4.0f) : 8.0f);
            
            // 1. Draw Liquid Capsule Background
            GraphicsPath capPath;
            float radius = capH / 2;
            capPath.AddArc((REAL)10, (REAL)capY, (REAL)(radius * 2), (REAL)(radius * 2), 90.0f, 180.0f);
            capPath.AddArc((REAL)(width - radius * 2 - 10), (REAL)capY, (REAL)(radius * 2), (REAL)(radius * 2), 270.0f, 180.0f);
            capPath.CloseFigure();
            SolidBrush glassBr(Color(100, 20, 20, 20));
            g.FillPath(&glassBr, &capPath);

            // 2. Dynamic Liquid Color
            Color cS = theme.grad_s, cE = theme.grad_e;
            if (isOvertime) { cS = Color(255, 230, 126, 34); cE = Color(255, 192, 57, 43); } // Intense Orange/Red for Overtime
            else if (app.elapsedSec < 300) { cS = Color(255, 241, 196, 15); cE = Color(255, 230, 126, 34); } // Urgency Orange
            
            // "Every 2 mins change color" - shift hue based on 2min cycle
            if ((int)(abs(app.elapsedSec) / 120) % 2 == 1) { swap(cS, cE); }

            // 3. Draw Animated Liquid Wave
            if (fillW > 2) {
                GraphicsPath wavePath;
                float waveAmp = 3.0f + cycle * 2.0f; // Wave gets stronger with cycle
                float waveFreq = 0.2f;
                float phase = (float)app.rotationIndex * 0.15f;
                
                wavePath.AddArc((REAL)10, (REAL)capY, (REAL)(radius * 2), (REAL)(radius * 2), 90.0f, 180.0f);
                wavePath.AddLine((REAL)(10 + radius), (REAL)capY, (REAL)(10 + fillW), (REAL)capY);
                for (float wy = capY; wy <= capY + capH; wy += 2) {
                    float wx = 10 + fillW + (float)sin(wy * waveFreq + phase) * waveAmp;
                    wavePath.AddLine((REAL)wx, (REAL)wy, (REAL)wx, (REAL)(wy + 2));
                }
                wavePath.AddLine((REAL)(10 + fillW), (REAL)(capY + capH), (REAL)(10 + radius), (REAL)(capY + capH));
                wavePath.CloseFigure();
                
                Region capReg(&capPath);
                g.SetClip(&capReg);
                LinearGradientBrush liquidBr(RectF(10, capY, fillW + waveAmp, capH), cS, cE, LinearGradientModeHorizontal);
                Color glowColors[] = { cS, theme.accent, cE };
                REAL offsets[] = { 0.0f, 0.5f, 1.0f };
                liquidBr.SetInterpolationColors(glowColors, offsets, 3);
                g.FillPath(&liquidBr, &wavePath);
                g.ResetClip();
                
                Pen glossPen(Color(100, 255, 255, 255), 1.0f);
                g.DrawPath(&glossPen, &capPath);
            }
        }


        if (cfg.showPet) {
            wstring pet = (app.running ? L"\U0001F63A" : L"\U0001F634");
            if (app.intensityLevel == 1) pet = L"\U0001F640"; else if (app.intensityLevel == 2) pet = L"\U0001F525";
            float pX = 10, pY = height - 26; if (app.running) { app.petBounce = (app.petBounce + 1) % 10; if (app.petBounce > 5) pY -= 2; }
            SolidBrush pBgBr(Color(60, theme.accent.GetR(), theme.accent.GetG(), theme.accent.GetB())); g.FillEllipse(&pBgBr, (REAL)(pX - 2), (REAL)(pY + 2), 22.0f, 22.0f);
            Font fE(L"Segoe UI Emoji", (app.compactLevel > 0 ? 15 : 18), FontStyleRegular, UnitPixel); g.DrawString(pet.c_str(), -1, &fE, PointF(pX, pY), &tBr);
        }
    } else {
        // Controls View
        StringFormat cf; cf.SetAlignment(StringAlignmentCenter); cf.SetLineAlignment(StringAlignmentCenter);
        if (app.compactLevel >= 2) {
            // Mode 2 & 3: Minimal Controls (Play, Compact, Close)
            float bw = 24, bh = 24, gap = 4; float sX = (width - (3 * bw + 2 * gap)) / 2; float y = (height - bh) / 2;
            wstring icons[] = { (app.running ? L"\uE769" : L"\uE768"), L"\uE712", L"\uE711" }; 
            Color iconColors[] = { theme.text, theme.accent, Color(255, 196, 43, 28) };
            for (int i = 0; i < 3; i++) {
                RectF br(sX + i * (bw + gap), y, bw, bh); SolidBrush bBg(Color(100, 100, 100, 100)); g.FillRectangle(&bBg, br);
                SolidBrush iBr(iconColors[i]); g.DrawString(icons[i].c_str(), -1, &fIcons, br, &cf, &iBr);
            }
        } else {
            // Mode 0 & 1: More Controls
            float bw = 18, bh = 22, gap = 1; float sX = (width - (11 * bw + 10 * gap)) / 2; float y = (height - bh) / 2;
            wstring icons[] = { (app.running ? L"\uE769" : L"\uE768"), (app.isTimer ? L"\uE916" : L"\uE706"), L"\uE701", L"\uE710", L"\uE771", L"\uE707", L"\uE70B", L"\uE7B3", L"\uE946", L"\uE712", L"\uE711" };
            Color iconColors[] = { theme.text, theme.text, (app.isPomodoro ? Color(255, 231, 76, 60) : theme.text), Color(255, 39, 174, 96), theme.text, theme.accent, theme.text, (app.showTask ? theme.accent : theme.dim), (app.isLiquidMode ? Color(255, 255, 215, 0) : theme.dim), theme.text, Color(255, 196, 43, 28) };
            for (int i = 0; i < 11; i++) {
                RectF br(sX + i * (bw + gap), y, bw, bh); SolidBrush bBg(Color(100, 100, 100, 100)); g.FillRectangle(&bBg, br);
                SolidBrush iBr(iconColors[i]); g.DrawString(icons[i].c_str(), -1, &fIcons, br, &cf, &iBr);
            }
        }
    }

    // Motivation Overlay
    if (app.motivationActive) {
        app.motivationTimer++;
        if (app.motivationTimer < 150) {
            BYTE alpha = (app.motivationTimer < 20) ? (BYTE)(app.motivationTimer * 12) : (app.motivationTimer > 130 ? (BYTE)((150 - app.motivationTimer) * 12) : 240);
            SolidBrush blurBr(Color(alpha, 0, 0, 0)); g.FillPath(&blurBr, &path);
            int cIdx = (app.motivationTimer / 5) % 7; Color rain[] = { Color(255,255,0,0), Color(255,255,127,0), Color(255,255,255,0), Color(255,0,255,0), Color(255,0,0,255), Color(255,75,0,130), Color(255,148,0,211) };
            SolidBrush mBr(Color(alpha, rain[cIdx].GetR(), rain[cIdx].GetG(), rain[cIdx].GetB())); 
            StringFormat mF; mF.SetAlignment(StringAlignmentCenter); mF.SetLineAlignment(StringAlignmentCenter);
            wstring msg = app.isRestPhase ? app.restMessages[app.currentMotivationIdx] : app.motivations[app.currentMotivationIdx];
            
            // Auto-size font to fit
            float startSize = (app.compactLevel >= 3) ? 10.0f : ((app.compactLevel > 0) ? 12.0f : 16.0f);
            float padX = (app.compactLevel >= 3) ? 4.0f : 8.0f;
            float padY = (app.compactLevel >= 3) ? 2.0f : 5.0f;
            RectF layout(padX, padY, (float)width - padX * 2, (float)height - padY * 2);
            while (startSize > 3.0f) {
                Font fTest(&ff, startSize, FontStyleBold, UnitPixel);
                RectF bndM; g.MeasureString(msg.c_str(), -1, &fTest, layout, &mF, &bndM);
                if (bndM.Width <= layout.Width && bndM.Height <= layout.Height) break;
                startSize -= 0.5f;
            }
            Font fMotiv(&ff, startSize, FontStyleBold, UnitPixel);
            g.DrawString(msg.c_str(), -1, &fMotiv, layout, &mF, &mBr);
        } else app.motivationActive = false;
    }

    POINT ptS = { 0, 0 }; SIZE szW = { width, height }; BLENDFUNCTION bl = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    POINT ptD; RECT wR; GetWindowRect(hwnd, &wR); ptD.x = wR.left; ptD.y = wR.top;
    if (app.wiggleFrames > 0) { app.wiggleFrames--; ptD.y += (int)(sin(app.wiggleFrames * 0.5) * 10); }
    UpdateLayeredWindow(hwnd, hdcS, &ptD, &szW, hdcM, &ptS, 0, &bl, ULW_ALPHA);
    DeleteObject(hbmM); DeleteDC(hdcM); ReleaseDC(hwnd, hdcS);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: srand((unsigned)time(NULL)); SetTimer(hwnd, 1, 30, NULL); return 0;
    case WM_TIMER:
        if (app.running) {
            auto now = chrono::steady_clock::now(); double d = chrono::duration<double>(now - app.lastUpdate).count();
            if (app.isTimer) app.elapsedSec -= d; else app.elapsedSec += d;
            app.lastUpdate = now; app.UpdateIntensity();
            
            if (app.isPomodoro && app.elapsedSec <= 0) {
                app.isRestPhase = !app.isRestPhase;
                app.elapsedSec = app.isRestPhase ? app.restTimeSec : app.workTimeSec;
                app.initialTimerSec = app.elapsedSec;
                app.TriggerMotivation();
                Beep(1000, 200);
            } else {
                int curM = (int)(abs(app.elapsedSec) / 60);
                if (curM != app.lastMinute) {
                    app.lastMinute = curM; app.TriggerMotivation();
                    if (curM % 5 == 0 && app.productivityMode) app.wiggleFrames = 60;
                    Beep(800, 50);
                }
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
            if (app.compactLevel >= 2) {
                float bw = 24, bh = 24, gap = 4; float sX = (w - (3 * bw + 2 * gap)) / 2; float bY = (h - bh) / 2;
                if (y >= bY && y <= bY + bh) {
                    if (x >= sX && x <= sX + bw) { if (app.running) app.running = false; else { app.running = true; app.lastUpdate = chrono::steady_clock::now(); } }
                    else if (x >= sX + bw + gap && x <= sX + 2 * bw + gap) { app.CycleCompactMode(); }
                    else if (x >= sX + 2 * bw + 2 * gap && x <= sX + 3 * bw + 2 * gap) { PostQuitMessage(0); }
                    return 0;
                }
            } else {
                float bw = 20, bh = 22, gap = 1; float sX = (w - (10 * bw + 9 * gap)) / 2; float bY = (h - bh) / 2;
                if (y >= bY && y <= bY + bh) {
                    int bI = -1; for (int i = 0; i < 11; i++) { float bx = sX + i * (bw + gap); if (x >= bx && x <= bx + bw) { bI = i; break; } }
                    if (bI == 0) { if (app.running) app.running = false; else { app.running = true; app.lastUpdate = chrono::steady_clock::now(); } }
                    else if (bI == 1) { if (!app.running) { app.isTimer = !app.isTimer; if (app.isTimer) { double mins = GetTimerInput(hwnd); app.elapsedSec = mins * 60.0; app.initialTimerSec = app.elapsedSec; } else app.elapsedSec = 0; } }
                    else if (bI == 2) { 
                        if (!app.running) {
                            app.isPomodoro = !app.isPomodoro; 
                            if (app.isPomodoro) {
                                double wM = GetTimerInput(hwnd, L"Work Time (min)");
                                double rM = GetTimerInput(hwnd, L"Rest Time (min)");
                                app.workTimeSec = wM * 60.0;
                                app.restTimeSec = rM * 60.0;
                                app.elapsedSec = app.workTimeSec;
                                app.initialTimerSec = app.elapsedSec;
                                app.isTimer = true;
                                app.isRestPhase = false;
                                app.running = true;
                                app.lastUpdate = chrono::steady_clock::now();
                            }
                        }
                    }
                    else if (bI == 3) { wchar_t p[MAX_PATH]; GetModuleFileName(NULL, p, MAX_PATH); ShellExecute(NULL, L"open", p, NULL, NULL, SW_SHOW); }
                    else if (bI == 4) { app.themeIndex = (app.themeIndex + 1) % app.themes.size(); app.UpdatePalette(); }
                    else if (bI == 5) { app.themeIndex = rand() % app.themes.size(); app.UpdatePalette(); }
                    else if (bI == 6) { wstring n = GetTaskInput(hwnd); if (!n.empty()) app.taskName = n; }
                    else if (bI == 7) { app.showTask = !app.showTask; }
                    else if (bI == 8) { app.isLiquidMode = !app.isLiquidMode; }
                    else if (bI == 9) { app.CycleCompactMode(); }
                    else if (bI == 10) PostQuitMessage(0);
                    return 0;
                }
            }
        }
        SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0); return 0;
    }
    case WM_RBUTTONDOWN: 
        app.CycleCompactMode(); 
        return 0;
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
    MSG m; while (GetMessage(&m, NULL, 0, 0)) { TranslateMessage(&m); DispatchMessage(&m); }
    GdiplusShutdown(gst); return 0;
}
