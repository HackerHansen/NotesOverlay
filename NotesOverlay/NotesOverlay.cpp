#include "Utils/Utils.h"

using namespace std;

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK KeyHook(const int code, const WPARAM wParam, const LPARAM lParam);

const uint8_t cycleKey = VK_NUMPAD2; // This is the key you want to use to cycle through your notes, change it if you want 
vector<vector<string>> notes;
int currentNotesIndex = 0;
HWND overlayHWND;
int width = 1920; // Will be changed later anyway so the values here don't really matter 
int height = 1080; // Will be changed later anyway so the values here don't really matter 
const int fontSize = 20; // Change this if you want 
PAINTSTRUCT ps;

// Epic font stuff, you can change this if you want the notes to look different 
// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createfontw 
HFONT textFont = CreateFontW(
    fontSize,            // Height 
    0,                   // Width (leave this at 0 for auto) 
    0,                   // Escapement 
    0,                   // Orientation 
    0,                   // Weight 
    false,               // Italic 
    false,               // Underline 
    false,               // StrikeOut (strikethrough) 
    DEFAULT_CHARSET,     // CharSet 
    OUT_DEVICE_PRECIS,   // OutPrecision
    CLIP_DEFAULT_PRECIS, // ClipPrecision
    ANTIALIASED_QUALITY, // Quality
    VARIABLE_PITCH,      // Pitch and Family 
    L"Cascadia Code");   // Font 

int main() { 
    SetConsoleTitleA("Notes Overlay");
    cout << "Choose a text file for your notes \n";
    // Prompt the user for an epic file for input 
    std::string path = "";
    while (path.empty()) path = Utils::promptUserForFilePath(L"Text File (*.txt)\0*.txt\0");
    notes = Utils::readFile(path);
    // Uncomment the following line if you want to get rid of the console window after selecting a file 
    //FreeConsole();
    // Set up keyboard hook and notes window 
    SetWindowsHookEx(WH_KEYBOARD_LL, KeyHook, NULL, 0);
    WNDCLASS wc = { 0 };
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); // Black 
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = NULL;
    wc.lpfnWndProc = OverlayWndProc;
    wc.lpszClassName = L"InsaneClassName";
    wc.style = CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClass(&wc))
        MessageBox(NULL, L"Oopsie, something went wrong! Try restarting the overlay.", L"Error", MB_OK);
    RECT desktopRect;
    HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktopRect);
    width = desktopRect.right - desktopRect.left;
    height = desktopRect.bottom - desktopRect.top;
    overlayHWND = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE,
        L"InsaneClassName",
        NULL,
        WS_POPUP | WS_EX_NOACTIVATE,
        desktopRect.left,
        desktopRect.top,
        width,
        height,
        NULL,
        NULL,
        NULL,
        NULL);
    SetLayeredWindowAttributes(overlayHWND, RGB(0, 0, 0), 0, LWA_COLORKEY);

    SetCapture(overlayHWND);
    ShowCursor(false);

    ShowWindow(overlayHWND, SW_RESTORE);

    MSG messages;
    while (GetMessage(&messages, NULL, 0, 0) > 0) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    DeleteObject(overlayHWND); //doing it just in case
    return messages.wParam;
}

void onFrameRender(HWND hwnd, HDC hdc) { // Not super optimized but the notes list size is probably small so it doesn't really matter that much 
    // Resize window 
    int maxWidth = 0;
    for (const std::string& lineText : notes.at(currentNotesIndex)) maxWidth = max(maxWidth, Utils::getTextWidth(hdc, lineText));
    RECT desktopRect;
    HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktopRect);
    width = desktopRect.right - desktopRect.left;
    height = desktopRect.bottom - desktopRect.top;
    MoveWindow(overlayHWND, (width / 2) - (maxWidth / 2), 30, maxWidth, fontSize * notes.at(currentNotesIndex).size(), false);
    // Draw 
    int currY = 0; 
    for (const std::string& lineText : notes.at(currentNotesIndex)) {
        int textWidth = Utils::getTextWidth(hdc, lineText);
        int startX = (maxWidth / 2) - (textWidth / 2);
        RenderUtils::fillRect(hdc, startX, currY, textWidth, fontSize, RGB(1, 1, 1));
        RenderUtils::renderText(hdc, lineText, startX, currY, RGB(15, 255, 15));
        currY += fontSize;
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_PAINT) {
        // Get hdc
        HDC hdc = BeginPaint(hwnd, &ps);

        // Set transparent background for text and stuff 
        int ogBkMode = GetBkMode(hdc);
        SetBkMode(hdc, TRANSPARENT);

        // Select objects 
        HGDIOBJ og = SelectObject(hdc, GetStockObject(DC_PEN));
        SelectObject(hdc, GetStockObject(DC_BRUSH));

        // Font stuff
        HFONT ogFont;
        ogFont = (HFONT)SelectObject(hdc, textFont);
        COLORREF ogTextColor = GetTextColor(hdc);

        // Call onFrameRender to draw stuff 
        onFrameRender(hwnd, hdc);

        // End 
        SetBkMode(hdc, ogBkMode);
        SelectObject(hdc, ogFont);
        SelectObject(hdc, og);
        SetTextColor(hdc, ogTextColor);
        EndPaint(hwnd, &ps);
        DeleteDC(hdc);
    }
    return DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT CALLBACK KeyHook(const int code, const WPARAM wParam, const LPARAM lParam) {
    if (wParam == WM_KEYDOWN) {
        if ((reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam))->vkCode == cycleKey) {
            if (++currentNotesIndex >= notes.size()) {
                currentNotesIndex = 0;
            }
            InvalidateRect(overlayHWND, NULL, true); 
            return 1; 
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}