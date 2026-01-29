#include <windows.h>
#include <cmath>
#include <thread>
#include <vector>
#include <chrono>

// Variáveis Globais de Controle e Dimensões
int w, h;
bool runAto1 = true;
bool runAto2 = true;
bool runAto3 = true;

// --- ESTRUTURAS E CONVERSÕES (HSL/HSV) ---
struct HSL { float h, s, l; };
struct HSV { float h, s, v; };

float hue2rgb(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f; if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

HSL RGBtoHSL(RGBQUAD rgb) {
    float r = rgb.rgbRed / 255.0f, g = rgb.rgbGreen / 255.0f, b = rgb.rgbBlue / 255.0f;
    float max = fmaxf(fmaxf(r, g), b), min = fminf(fminf(r, g), b);
    float h, s, l = (max + min) / 2.0f;
    if (max == min) h = s = 0.0f;
    else {
        float diff = max - min;
        s = (l > 0.5f) ? diff / (2.0f - max - min) : diff / (max + min);
        if (max == r) h = (g - b) / diff + (g < b ? 6.0f : 0.0f);
        else if (max == g) h = (b - r) / diff + 2.0f;
        else h = (r - g) / diff + 4.0f;
        h /= 6.0f;
    }
    return { h, s, l };
}

RGBQUAD HSLtoRGB(HSL hsl) {
    float r, g, b, q, p;
    if (hsl.s == 0.0f) r = g = b = hsl.l;
    else {
        q = (hsl.l < 0.5f) ? (hsl.l * (1.0f + hsl.s)) : (hsl.l + hsl.s - hsl.l * hsl.s);
        p = 2.0f * hsl.l - q;
        r = hue2rgb(p, q, hsl.h + 1.0f / 3.0f);
        g = hue2rgb(p, q, hsl.h);
        b = hue2rgb(p, q, hsl.h - 1.0f / 3.0f);
    }
    return { (BYTE)(b * 255), (BYTE)(g * 255), (BYTE)(r * 255), 0 };
}

HSV RGBtoHSV(RGBQUAD rgb) {
    float r = rgb.rgbRed / 255.0f, g = rgb.rgbGreen / 255.0f, b = rgb.rgbBlue / 255.0f;
    float max = fmaxf(fmaxf(r, g), b), min = fminf(fminf(r, g), b);
    float h, s, v = max;
    float d = max - min;
    s = max == 0 ? 0 : d / max;
    if (max == min) h = 0;
    else {
        if (max == r) h = (g - b) / d + (g < b ? 6 : 0);
        else if (max == g) h = (b - r) / d + 2;
        else h = (r - g) / d + 4;
        h /= 6;
    }
    return { h * 360.0f, s, v };
}

RGBQUAD HSVtoRGB(HSV hsv) {
    float r, g, b;
    int i = floor(hsv.h / 60.0f);
    float f = hsv.h / 60.0f - i;
    float p = hsv.v * (1 - hsv.s);
    float q = hsv.v * (1 - f * hsv.s);
    float t = hsv.v * (1 - (1 - f) * hsv.s);
    switch (i % 6) {
    case 0: r = hsv.v, g = t, b = p; break;
    case 1: r = q, g = hsv.v, b = p; break;
    case 2: r = p, g = hsv.v, b = t; break;
    case 3: r = p, g = q, b = hsv.v; break;
    case 4: r = t, g = p, b = hsv.v; break;
    case 5: r = hsv.v, g = p, b = q; break;
    }
    return { (BYTE)(b * 255), (BYTE)(g * 255), (BYTE)(r * 255), 0 };
}

// --- ATO 1: HELP ME ---
void ShaderPayload() {
    HDC hdc = GetDC(NULL); HDC hdcCopy = CreateCompatibleDC(hdc);
    BITMAPINFO bmpi = { 0 }; bmpi.bmiHeader.biSize = sizeof(bmpi);
    bmpi.bmiHeader.biWidth = w; bmpi.bmiHeader.biHeight = -h;
    bmpi.bmiHeader.biPlanes = 1; bmpi.bmiHeader.biBitCount = 32;
    RGBQUAD* rgbquad; HBITMAP bmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
    SelectObject(hdcCopy, bmp);
    while (runAto1) {
        hdc = GetDC(NULL); StretchBlt(hdcCopy, 0, 0, w, h, hdc, 0, 0, w, h, SRCCOPY);
        for (int i = 0; i < w * h; i++) {
            HSL hsl = RGBtoHSL(rgbquad[i]); hsl.h = fmod(hsl.h + 0.01f, 1.0f);
            rgbquad[i] = HSLtoRGB(hsl);
        }
        StretchBlt(hdc, 0, 0, w, h, hdcCopy, 0, 0, w, h, SRCCOPY);
        ReleaseDC(NULL, hdc); Sleep(10);
    }
    DeleteObject(bmp); DeleteDC(hdcCopy);
}
void TextPayload() {
    while (runAto1) {
        HDC hdc = GetDC(NULL); SetTextColor(hdc, RGB(rand() % 255, rand() % 255, rand() % 255));
        SetBkMode(hdc, TRANSPARENT); TextOutA(hdc, rand() % w, rand() % h, "HELP ME", 5);
        ReleaseDC(NULL, hdc); Sleep(100);
    }
}

// --- ATO 2: BALLS & GLITCH ---
void BallPayload() {
    int x = 10, y = 10, sx = 1, sy = 1;
    while (runAto2) {
        HDC hdc = GetDC(0); x += 10 * sx; y += 10 * sy;
        HBRUSH b = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
        SelectObject(hdc, b); Ellipse(hdc, x, y, x + 100, y + 100);
        if (y >= h || y <= 0) sy *= -1; if (x >= w || x <= 0) sx *= -1;
        DeleteObject(b); ReleaseDC(0, hdc); Sleep(10);
    }
}
void GlitchPayload() {
    while (runAto2) {
        HDC hdc = GetDC(0); int x = rand() % w;
        BitBlt(hdc, x, 1, 10, h, hdc, x, 0, SRCCOPY);
        ReleaseDC(0, hdc); Sleep(2);
    }
}

// --- ATO 3: CAOS TOTAL ---
void Ato3_Melt() {
    while (runAto3) {
        HDC hdc = GetDC(0); int x = rand() % w;
        BitBlt(hdc, x, 1, 10, h, hdc, x, 0, NOTSRCCOPY); // Reverse Melt
        BitBlt(hdc, x, 10, 100, h, hdc, x, 0, SRCCOPY); // Simple Melt
        ReleaseDC(0, hdc); Sleep(5);
    }
}
void Ato3_Strobe() {
    while (runAto3) {
        HDC hdc = GetDC(0); PatBlt(hdc, 0, 0, w, h, PATINVERT);
        BitBlt(hdc, rand() % 10, rand() % 10, w, h, hdc, rand() % 10, rand() % 10, SRCCOPY); // Jitter
        ReleaseDC(0, hdc); Sleep(100);
    }
}

// --- ATO 4: PLASMA & ADVANCED ---
void PlasmaThread() {
    int ws = w / 4, hs = h / 4;
    HDC dc = GetDC(NULL); HDC dcCopy = CreateCompatibleDC(dc);
    BITMAPINFO bmpi = { 0 }; bmpi.bmiHeader.biSize = sizeof(bmpi);
    bmpi.bmiHeader.biWidth = ws; bmpi.bmiHeader.biHeight = -hs;
    bmpi.bmiHeader.biPlanes = 1; bmpi.bmiHeader.biBitCount = 32;
    RGBQUAD* rgbquad; HBITMAP bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
    SelectObject(dcCopy, bmp);
    float a = 5.0, b = 3.0;
    for (int i = 0;; i++) {
        HDC hdc = GetDC(NULL); StretchBlt(dcCopy, 0, 0, ws, hs, hdc, 0, 0, w, h, SRCCOPY);
        int randx = rand() % ws, randy = rand() % hs;
        for (int j = 0; j < ws * hs; j++) {
            int x = j % ws, y = j / ws;
            float zx = pow(x - randx, 2) / (a * a);
            float zy = pow(y - randy, 2) / (b * b);
            int fx = 128.0 + (128.0 * sin(sqrt(zx + zy) / 6.0));
            HSV hsv = RGBtoHSV(rgbquad[j]); hsv.h = fmod(fx + i, 360.0);
            rgbquad[j] = HSVtoRGB(hsv);
        }
        StretchBlt(hdc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);
        ReleaseDC(NULL, hdc); Sleep(30);
    }
}
void DistortionThread() {
    while (true) {
        HDC hdc = GetDC(0); int x = rand() % w;
        BitBlt(hdc, x, 1, 10, h, hdc, x, 0, SRCCOPY);
        BitBlt(hdc, rand() % 3, rand() % 3, w, h, hdc, 0, 0, SRCCOPY);
        ReleaseDC(0, hdc); Sleep(5);
    }
}
void StrobeThread() {
    while (true) {
        HDC hdc = GetDC(0); PatBlt(hdc, 0, 0, w, h, PATINVERT);
        ReleaseDC(0, hdc); Sleep(150);
    }
}
void BouncingThread() {
    int x = 10, y = 10, sx = 1, sy = 1, inc = 15;
    while (true) {
        HDC hdc = GetDC(0); x += inc * sx; y += inc * sy;
        HBRUSH brush = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
        SelectObject(hdc, brush); Ellipse(hdc, x, y, x + 120, y + 120);
        if (y >= h - 120 || y <= 0) sy *= -1; if (x >= w - 120 || x <= 0) sx *= -1;
        DeleteObject(brush); ReleaseDC(0, hdc); Sleep(15);
    }
}

// --- CONTROLE PRINCIPAL ---
int main() {
    FreeConsole();
    w = GetSystemMetrics(0); h = GetSystemMetrics(1);

    // ATO 1
    std::thread a1_1(ShaderPayload); std::thread a1_2(TextPayload);
    std::this_thread::sleep_for(std::chrono::seconds(60));
    runAto1 = false; a1_1.join(); a1_2.join();

    // ATO 2
    std::thread a2_1(BallPayload); std::thread a2_2(GlitchPayload);
    std::this_thread::sleep_for(std::chrono::seconds(60));
    runAto2 = false; a2_1.join(); a2_2.join();

    // ATO 3
    std::thread a3_1(Ato3_Melt); std::thread a3_2(Ato3_Strobe);
    std::this_thread::sleep_for(std::chrono::seconds(60));
    runAto3 = false; a3_1.join(); a3_2.join();

    // ATO 4 (FINAL INFINITO)
    std::thread t1(PlasmaThread); std::thread t2(DistortionThread);
    std::thread t3(StrobeThread); std::thread t4(BouncingThread);

    t1.join(); t2.join(); t3.join(); t4.join();
    return 0;
}