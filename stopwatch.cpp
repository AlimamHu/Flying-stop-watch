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

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Msimg32.lib")

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

struct SubTask {
    wstring title;
    bool completed = false;
    double timeSec = 0;
    float bounce = 0; // Animation state
};

struct MainTask {
    wstring title;
    vector<SubTask> subtasks;
    int activeSubTaskIdx = -1;
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
    float pulseTime = 0;
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

    // Tasks & Drawer
    vector<MainTask> tasks;
    int activeTaskIdx = -1;
    bool drawerOpen = false;
    int baseHeight = 70;
    int expandedHeight = 250;
    int currentHeight = 70;
    int scrollOffset = 0;
    int finishFlash = 0; // For green flash feedback

    // Renaming state
    HWND editHwnd = NULL;
    int renamingTaskIdx = -1;
    int renamingSubIdx = -1;

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

    wstring GetTimeStr(double seconds) {
        int s = (int)abs(seconds);
        int h = s / 3600;
        int m = (s % 3600) / 60;
        int sec = s % 60;
        if (h > 0) return to_wstring(h) + L"h " + to_wstring(m) + L"m";
        if (m > 0) return to_wstring(m) + L"m " + to_wstring(sec) + L"s";
        return to_wstring(sec) + L"s";
    }
    
    void SetTimerFromInput(wstring input) {
        try {
            // Check for "min" or "minute" or "m"
            size_t mPos = input.find(L"min");
            if (mPos == wstring::npos) mPos = input.find(L"m");
            
            if (input.find(L":") != wstring::npos) {
                int min = stoi(input.substr(0, input.find(L":")));
                int sec = stoi(input.substr(input.find(L":") + 1));
                elapsedSec = min * 60 + sec;
            } else if (mPos != wstring::npos) {
                elapsedSec = stoi(input.substr(0, mPos)) * 60;
            } else {
                elapsedSec = stoi(input) * 60;
            }
        } catch(...) { elapsedSec = 25 * 60; }
        running = false;
    }

    vector<wstring> quotes = {
        L"Stay focused, stay driven.",
        L"One task at a time.",
        L"Discipline equals freedom.",
        L"Make every second count.",
        L"The best time to start is now.",
        L"Keep pushing forward.",
        L"Success is a habit."
    };
    int currentQuoteIdx = 0;

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
        for (int i = n - 1; i >= 0; i--) {
            palette.push_back(palette[i]);
        }
    }

    void UpdateIntensity() {
        double mins = elapsedSec / 60.0;
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
        int h = (int)(sTotal / 3600);
        int m = (int)((sTotal - h * 3600) / 60);
        int s = (int)(sTotal - h * 3600 - m * 60);
        wchar_t b[64];
        swprintf(b, L"%ls%02d:%02d:%02d", (elapsedSec < 0 ? L"-" : L""), h, m, s);
        return b;
    }
    
    wstring GetMsString() {
        int ms = (int)((abs(elapsedSec) - floor(abs(elapsedSec))) * 100);
        wchar_t b[16];
        swprintf(b, L".%02d", ms);
        return b;
    }

    void TriggerMotivation() {
        motivationActive = true;
        motivationTimer = 0;
        currentMotivationIdx = rand() % motivations.size();
    }
};

StopwatchApp app;

// --- DRAWING ENGINE ---

void Render(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    HDC hdcScreen = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdcMem, hbmMem);

    Graphics g(hdcMem);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit); // Max sharpness

    Theme& theme = app.themes[app.themeIndex];
    int r = 12;

    g.Clear(Color(0, 0, 0, 0));

    // 1. Background
    GraphicsPath path;
    path.AddArc(2, 2, r * 2, r * 2, 180, 90);
    path.AddArc(width - r * 2 - 2, 2, r * 2, r * 2, 270, 90);
    path.AddArc(width - r * 2 - 2, height - r * 2 - 2, r * 2, r * 2, 0, 90);
    path.AddArc(2, height - r * 2 - 2, r * 2, r * 2, 90, 90);
    path.CloseFigure();

    SolidBrush bgBrush(theme.bg);
    g.FillPath(&bgBrush, &path);

    // 2. Animated Border
    if (app.running) app.rotationIndex = (app.rotationIndex + 1) % app.palette.size();
    Color borderColor = app.palette[app.rotationIndex % app.palette.size()];
    Pen borderPen(borderColor, 2.5f);
    g.DrawPath(&borderPen, &path);

    // 3. Hover Effect
    if (app.isHovered) {
        SolidBrush dimBrush(Color(200, 0, 0, 0));
        g.FillPath(&dimBrush, &path);
    }

    // 4. Fonts
    FontFamily ff(L"Segoe UI");
    Font fontMain(&ff, 26, FontStyleBold, UnitPixel);
    Font fontSmall(&ff, 12, FontStyleRegular, UnitPixel);
    Font fontEmoji(L"Segoe UI Emoji", 18, FontStyleRegular, UnitPixel);
    Font fontMotiv(&ff, 10, FontStyleBold, UnitPixel);
    Font fontIcons(L"Segoe MDL2 Assets", 14, FontStyleRegular, UnitPixel);

    SolidBrush textBrush(theme.text);
    SolidBrush dimTextBrush(theme.dim);
    SolidBrush accentBrush(theme.accent);

    if (!app.isHovered) {
        // Normal View
        wstring t = app.GetTimeString();
        wstring ms = app.GetMsString();
        RectF bounds;
        g.MeasureString(t.c_str(), -1, &fontMain, PointF(0, 0), &bounds);
        
        float tx = (width - bounds.Width) / 2 - 5;
        float ty = (app.baseHeight - bounds.Height) / 2 - 5;

        // Flash Green if just finished
        SolidBrush greenBrush(Color(255, 0, 255, 0));
        SolidBrush* activeTimeBrush = (app.finishFlash > 0 ? &greenBrush : &textBrush);
        
        // Update Animations
        for(auto& t : app.tasks) {
            for(auto& st : t.subtasks) {
                if (st.bounce > 0) st.bounce -= 0.5f;
            }
        }
        if (app.finishFlash > 0) app.finishFlash--;

        g.DrawString(t.c_str(), -1, &fontMain, PointF(tx, ty), activeTimeBrush);
        g.DrawString(ms.c_str(), -1, &fontSmall, PointF(tx + bounds.Width - 2, ty + 12), &accentBrush);
        
        // Active Task Title & Quote
        if (app.activeTaskIdx != -1) {
            wstring taskInfo = app.tasks[app.activeTaskIdx].title;
            if (app.tasks[app.activeTaskIdx].activeSubTaskIdx != -1 && app.tasks[app.activeTaskIdx].activeSubTaskIdx < (int)app.tasks[app.activeTaskIdx].subtasks.size()) {
                taskInfo += L" : " + app.tasks[app.activeTaskIdx].subtasks[app.tasks[app.activeTaskIdx].activeSubTaskIdx].title;
            }
            StringFormat sf; sf.SetAlignment(StringAlignmentCenter);
            g.DrawString(taskInfo.c_str(), -1, &fontMotiv, RectF(0, 5, (float)width, 15), &sf, &dimTextBrush);
            
            // Show Quote higher and centered
            float blink = (float)(0.6 + 0.4 * sin(app.pulseTime * 3.0f));
            Color c1 = theme.grad_s; Color c2 = theme.grad_e;
            Color b1( (int)(255 * blink), c1.GetR(), c1.GetG(), c1.GetB());
            Color b2( (int)(255 * blink), c2.GetR(), c2.GetG(), c2.GetB());
            
            LinearGradientBrush gradBrush(RectF(0, app.baseHeight - 16, (float)width, 10), b1, b2, LinearGradientModeHorizontal);
            g.DrawString(app.quotes[app.currentQuoteIdx].c_str(), -1, &fontMotiv, RectF(0, app.baseHeight - 16, (float)width, 10), &sf, &gradBrush);
        }
    } else {
        // Controls View (6 Buttons)
        StringFormat centerF; centerF.SetAlignment(StringAlignmentCenter); centerF.SetLineAlignment(StringAlignmentCenter);
        float bw = 28, bh = 28, gap = 4;
        float startX = (width - (6 * bw + 5 * gap)) / 2;
        float y = (app.baseHeight - bh) / 2;
        wstring icons[] = { (app.running ? L"\uE769" : L"\uE768"), (app.isTimer ? L"\uE916" : L"\uE706"), L"\uE771", L"\uE707", L"\uE8FD", L"\uE711" };
        Color iconColors[] = { theme.text, theme.text, theme.text, theme.accent, theme.text, Color(255, 196, 43, 28) };
        for (int i = 0; i < 6; i++) {
            RectF btnRect(startX + i * (bw + gap), y, bw, bh);
            SolidBrush btnBg(Color(100, 100, 100, 100));
            g.FillRectangle(&btnBg, btnRect);
            SolidBrush iconBrush(iconColors[i]);
            g.DrawString(icons[i].c_str(), -1, &fontIcons, btnRect, &centerF, &iconBrush);
        }
    }

    // 5. Draw Drawer Content (Scrollable)
    if (app.currentHeight > app.baseHeight) {
        float listY = (float)app.baseHeight + 5;
        Pen dimPen(&dimTextBrush, 1.0f);
        g.DrawLine(&dimPen, 10.0f, listY - 2, (float)width - 10, listY - 2);
        
        // Drawer Header
        g.DrawString(L"PROJECTS", -1, &fontMotiv, PointF(15, listY), &accentBrush);
        g.DrawString(L"+ TASK", -1, &fontMotiv, PointF((float)width - 45, listY), &dimTextBrush);
        
        // Spacing adjustment - Quote removed from here as per request
        RectF clipRect(5, listY + 15, (float)width - 10, (float)app.currentHeight - listY - 20);
        g.SetClip(clipRect);

        float itemY = listY + 20 - app.scrollOffset;
        for (int i = 0; i < (int)app.tasks.size(); i++) {
            MainTask& mt = app.tasks[i];
            bool isActive = (i == app.activeTaskIdx);
            
            // Draw Main Task Icon
            g.DrawString(isActive ? L"◈" : L"◇", -1, &fontSmall, PointF(10, itemY), (isActive ? &textBrush : &dimTextBrush));
            g.DrawString(mt.title.c_str(), -1, &fontSmall, PointF(25, itemY), (isActive ? &textBrush : &dimTextBrush));
            
            double total = 0; for(auto& st : mt.subtasks) total += st.timeSec;
            wstring mTime = app.GetTimeStr(total);
            g.DrawString(mTime.c_str(), -1, &fontMotiv, PointF((float)width - 85, itemY + 2), (isActive ? &textBrush : &dimTextBrush));
            
            // Subtask Add Button
            g.DrawString(L"\uE109", -1, &fontIcons, PointF((float)width - 40, itemY + 2), &dimTextBrush);
            
            // Task Finish Button (Checkmark)
            g.DrawString(L"\uE73E", -1, &fontIcons, PointF((float)width - 20, itemY + 2), (isActive ? &accentBrush : &dimTextBrush));

            itemY += 20;
            for (int j = 0; j < (int)mt.subtasks.size(); j++) {
                SubTask& st = mt.subtasks[j];
                bool subActive = (isActive && j == mt.activeSubTaskIdx);
                
                // Animated Checkbox
                float b = st.bounce;
                RectF checkRect(25 - b/2, itemY + 2 - b/2, 12 + b, 12 + b);
                Pen checkPen(subActive ? theme.text : theme.dim, 1.0f);
                g.DrawRectangle(&checkPen, checkRect);
                if (st.completed) {
                    SolidBrush checkBrush(theme.accent);
                    g.FillRectangle(&checkBrush, RectF(checkRect.X + 2, checkRect.Y + 2, checkRect.Width - 4, checkRect.Height - 4));
                }

                g.DrawString(st.title.c_str(), -1, &fontMotiv, PointF(42, itemY + 2), (subActive ? &textBrush : &dimTextBrush));
                
                if (mt.subtasks[j].timeSec > 0 || subActive) {
                    g.DrawString(app.GetTimeStr(mt.subtasks[j].timeSec).c_str(), -1, &fontMotiv, PointF((float)width - 85, itemY + 2), &dimTextBrush);
                }
                itemY += 18;
            }
            itemY += 5;
        }
        g.ResetClip();
    }

    POINT ptSrc = { 0, 0 };
    SIZE sizeWnd = { width, height };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    POINT ptDest;
    RECT wrc; GetWindowRect(hwnd, &wrc);
    ptDest.x = wrc.left; ptDest.y = wrc.top;

    UpdateLayeredWindow(hwnd, hdcScreen, &ptDest, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdcScreen);
}

// --- WINDOW PROC ---

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        srand((unsigned)time(NULL));
        SetTimer(hwnd, 1, 30, NULL);
        return 0;
    case WM_TIMER: {
        if (app.running) {
            auto now = chrono::steady_clock::now();
            double delta = chrono::duration<double>(now - app.lastUpdate).count();
            if (app.isTimer) app.elapsedSec -= delta;
            else app.elapsedSec += delta;
            app.lastUpdate = now;
            app.UpdateIntensity();
            
            // Update Active Task Time
            if (app.activeTaskIdx != -1) {
                MainTask& mt = app.tasks[app.activeTaskIdx];
                if (mt.activeSubTaskIdx != -1) {
                    mt.subtasks[mt.activeSubTaskIdx].timeSec += delta;
                }
            }

            int curMin = (int)(abs(app.elapsedSec) / 60);
            if (curMin != app.lastMinute) {
                app.lastMinute = curMin;
                app.TriggerMotivation();
                Beep(800, 50);
            }
        }

        // Handle Animations
        app.pulseTime += 0.03f;
        
        // Handle Drawer Animation
        int targetH = app.drawerOpen ? app.expandedHeight : app.baseHeight;
        if (app.currentHeight != targetH) {
            if (app.currentHeight < targetH) app.currentHeight = min(targetH, app.currentHeight + 15);
            else app.currentHeight = max(targetH, app.currentHeight - 15);
            
            RECT wrc; GetWindowRect(hwnd, &wrc);
            SetWindowPos(hwnd, NULL, wrc.left, wrc.top, 210, app.currentHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }

        Render(hwnd);
        return 0;
    }

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wp);
        app.scrollOffset = max(0, app.scrollOffset - (delta / 2));
        return 0;
    }
    case WM_LBUTTONDBLCLK: {
        int x = LOWORD(lp);
        int y = HIWORD(lp);
        wstring initialText = L"";
        if (y > app.baseHeight) {
            float listY = (float)app.baseHeight + 5;
            float itemY = listY + 20 - app.scrollOffset;
            for (int i = 0; i < (int)app.tasks.size(); i++) {
                if (y >= itemY && y <= itemY + 20) {
                    app.renamingTaskIdx = i; app.renamingSubIdx = -1;
                    initialText = app.tasks[i].title;
                    goto open_edit;
                }
                itemY += 20;
                for (int j = 0; j < (int)app.tasks[i].subtasks.size(); j++) {
                    if (y >= itemY && y <= itemY + 18) {
                        app.renamingTaskIdx = i; app.renamingSubIdx = j;
                        initialText = app.tasks[i].subtasks[j].title;
                        goto open_edit;
                    }
                    itemY += 18;
                }
                itemY += 5;
            }
        }
        return 0;
        open_edit:
        if (app.editHwnd) DestroyWindow(app.editHwnd);
        app.editHwnd = CreateWindowEx(0, L"EDIT", initialText.c_str(), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 
                                     x, y, 120, 22, hwnd, (HMENU)101, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
        SendMessage(app.editHwnd, EM_SETSEL, 0, -1);
        SetFocus(app.editHwnd);
        return 0;
    }
    case WM_COMMAND: {
        if (LOWORD(wp) == 101) {
            if (HIWORD(wp) == EN_KILLFOCUS) {
                DestroyWindow(app.editHwnd); app.editHwnd = NULL;
                app.renamingTaskIdx = -1;
            } else if (HIWORD(wp) == EN_CHANGE) {
                wchar_t buf[256]; GetWindowText(app.editHwnd, buf, 256);
                if (app.renamingTaskIdx != -1) {
                    if (app.renamingSubIdx == -1) app.tasks[app.renamingTaskIdx].title = buf;
                    else app.tasks[app.renamingTaskIdx].subtasks[app.renamingSubIdx].title = buf;
                } else {
                    app.SetTimerFromInput(buf);
                }
                Render(hwnd); // Real-time update
            }
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        int x = LOWORD(lp);
        int y = HIWORD(lp);
        RECT rc; GetClientRect(hwnd, &rc);
        int width = rc.right - rc.left;

        if (app.isHovered && y < app.baseHeight) {
            float bw = 28, bh = 28, gap = 4;
            float startX = (width - (6 * bw + 5 * gap)) / 2;
            float btnY = (app.baseHeight - bh) / 2;
            if (y >= btnY && y <= btnY + bh) {
                int btnIdx = -1;
                for (int i = 0; i < 6; i++) {
                    float bx = startX + i * (bw + gap);
                    if (x >= bx && x <= bx + bw) { btnIdx = i; break; }
                }
                if (btnIdx == 0) { // Play/Pause
                    if (app.running) app.running = false;
                    else { app.running = true; app.lastUpdate = chrono::steady_clock::now(); }
                } else if (btnIdx == 1) { // Mode
                    app.isTimer = !app.isTimer; app.elapsedSec = 0; app.running = false;
                } else if (btnIdx == 2) { // Theme
                    app.themeIndex = (app.themeIndex + 1) % app.themes.size(); app.UpdatePalette();
                } else if (btnIdx == 3) { // Random Theme
                    app.themeIndex = rand() % app.themes.size(); app.UpdatePalette();
                    app.currentQuoteIdx = rand() % app.quotes.size();
                } else if (btnIdx == 4) { // Toggle Drawer
                    app.drawerOpen = !app.drawerOpen;
                } else if (btnIdx == 5) { // Exit
                    PostQuitMessage(0);
                }
                Beep(800, 30);
                return 0;
            }
        } 
        
        if (app.drawerOpen && y >= app.baseHeight) {
            float listY = (float)app.baseHeight + 5;
            if (x > width - 50 && y < app.baseHeight + 25) {
                MainTask nt; nt.title = L"NEW TASK"; app.tasks.push_back(nt);
                if (app.activeTaskIdx == -1) app.activeTaskIdx = (int)app.tasks.size() - 1;
            } else {
                float itemY = listY + 20 - app.scrollOffset;
                for (int i = 0; i < (int)app.tasks.size(); i++) {
                    // Task Selection
                    if (y >= itemY && y <= itemY + 20) { 
                        app.activeTaskIdx = i; 
                        if (app.tasks[i].activeSubTaskIdx == -1 && !app.tasks[i].subtasks.empty()) app.tasks[i].activeSubTaskIdx = 0;
                        app.running = true; app.lastUpdate = chrono::steady_clock::now();
                        return 0; // Handled, stop fall-through to dragging
                    }
                    // Add Subtask button
                    if (x > width - 45 && x < width - 25 && y >= itemY && y <= itemY + 20) {
                        app.tasks[i].subtasks.push_back({L"New Subtask"});
                        if (app.tasks[i].activeSubTaskIdx == -1) app.tasks[i].activeSubTaskIdx = 0;
                        return 0;
                    }
                    // Finish Main Task button
                    if (x > width - 25 && y >= itemY && y <= itemY + 20) {
                        for(auto& st : app.tasks[i].subtasks) st.completed = true;
                        app.finishFlash = 20; Beep(1200, 100);
                        return 0;
                    }
                    
                    itemY += 20;
                    for (int j = 0; j < (int)app.tasks[i].subtasks.size(); j++) {
                        if (y >= itemY && y <= itemY + 18) { 
                            app.activeTaskIdx = i; app.tasks[i].activeSubTaskIdx = j; 
                            app.running = true; app.lastUpdate = chrono::steady_clock::now();
                            if (x < 50) { // Click on checkbox
                                app.tasks[i].subtasks[j].completed = !app.tasks[i].subtasks[j].completed;
                                app.tasks[i].subtasks[j].bounce = 4.0f; // Start bounce
                                Beep(600, 20);
                            }
                            return 0; 
                        }
                        itemY += 18;
                    }
                    itemY += 5;
                }
            }
        } else if (y < app.baseHeight) {
            if (app.isTimer && !app.running) {
                app.renamingTaskIdx = -1;
                if (app.editHwnd) DestroyWindow(app.editHwnd);
                app.editHwnd = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 
                                             x, y, 60, 22, hwnd, (HMENU)101, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
                SetFocus(app.editHwnd);
                return 0;
            }

            if (app.activeTaskIdx != -1) {
                MainTask& mt = app.tasks[app.activeTaskIdx];
                if (mt.activeSubTaskIdx != -1 && mt.activeSubTaskIdx < (int)mt.subtasks.size()) {
                    mt.subtasks[mt.activeSubTaskIdx].completed = true;
                    mt.activeSubTaskIdx++;
                    app.running = false; 
                    app.finishFlash = 20; 
                    Beep(1200, 100);
                    app.isHovered = false; 
                }
            } else {
                SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            }
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
        TrackMouseEvent(&tme);
        if (!app.isHovered) { app.isHovered = true; Render(hwnd); }
        return 0;
    }
    case WM_MOUSELEAVE:
        app.isHovered = false;
        Render(hwnd);
        return 0;
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) PostQuitMessage(0);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hI;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"StopwatchProCPP";
    wc.style = CS_DBLCLKS;
    RegisterClass(&wc);

    int w = 210, h = 70;
    int sx = GetSystemMetrics(SM_CXSCREEN) - w - 40;
    int sy = 40;

    app.hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW, wc.lpszClassName, L"Stopwatch Pro",
                               WS_POPUP | WS_VISIBLE, sx, sy, w, h, NULL, NULL, hI, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }

    GdiplusShutdown(gdiplusToken);
    return 0;
}
