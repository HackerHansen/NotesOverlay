#include "Utils.h"

int Utils::getTextWidth(HDC hdc, std::string txt) {
    int res = 0;
    for (char c : txt) {
        SIZE s;
        GetTextExtentPoint32(hdc, std::wstring(1, c).c_str(), 1, &s);
        res += s.cx;
    }
    return res;
}

std::string Utils::promptUserForFilePath(LPCWSTR types) {
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = types;
    ofn.lpstrFile = (LPWSTR)szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = (LPCWSTR)L"";
    return GetOpenFileName(&ofn) ? (std::string)CW2A(ofn.lpstrFile) : "";
}

std::vector<std::vector<std::string>> Utils::readFile(std::string path) {
    std::vector<std::vector<std::string>> res;
    std::ifstream fin(path);
    char last = ' ';
    char in = ' ';
    std::vector<std::string> chunk;
    std::string curr = "";
    while (fin.get(in)) {
        if (in != '\n') curr += in;
        else {
            if (last == '\n') {
                if (!curr.empty()) chunk.push_back(curr);
                if (!chunk.empty()) res.push_back(chunk);
                chunk.clear();
            }
            else {
                if (!curr.empty()) chunk.push_back(curr);
            }
            curr.clear();
        }
        last = in;
    }
    if (!curr.empty()) chunk.push_back(curr);
    if (!chunk.empty()) res.push_back(chunk);
    return res;
}

void RenderUtils::fillRect(HDC hdc, int x, int y, int w, int h, COLORREF color) {
    SetDCPenColor(hdc, color);
    SetDCBrushColor(hdc, color);
    Rectangle(hdc, x, y, x + w, y + h);
};

void RenderUtils::renderText(HDC hdc, std::string txt, int x, int y, COLORREF color) {
    SetTextColor(hdc, color);
    TextOut(hdc, x, y, std::wstring(txt.begin(), txt.end()).c_str(), txt.length());
};