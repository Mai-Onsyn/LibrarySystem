module;
#include <iostream>
#include <iomanip>
export module ConsoleDrawer;
import Types;
import Format;

using std::setw;
using std::left;
using std::right;
using std::internal;
using std::endl;

void repeat(const String &str, const Int count) {
    for (Int i = 0; i < count; i++) {
        cout << str;
    }
}

void repeat(const char ch, const Int count) {
    for (Int i = 0; i < count; i++) {
        cout << ch;
    }
}


Int calcDisplayWidth(const std::string_view u8str) {
    int width = 0;
    for (Long i = 0; i < u8str.size(); ) {
        unsigned char c = u8str[i];
        int bytes = 0;
        uint32_t codepoint = 0;
        if ((c & 0x80) == 0) {          // ASCII
            codepoint = c;
            bytes = 1;
        } else if ((c & 0xE0) == 0xC0) { // 2 bytes
            codepoint = c & 0x1F;
            bytes = 2;
        } else if ((c & 0xF0) == 0xE0) { // 3 bytes 大多数汉字
            codepoint = c & 0x0F;
            bytes = 3;
        } else if ((c & 0xF8) == 0xF0) { // 4 bytes
            codepoint = c & 0x07;
            bytes = 4;
        } else {
            bytes = 1; // 错误处理
        }
        for (int j = 1; j < bytes; ++j) {
            if (i + j >= u8str.size()) break;
            codepoint = (codepoint << 6) | (u8str[i + j] & 0x3F);
        }
        i += bytes;
        if ((codepoint >= 0x4E00 && codepoint <= 0x9FFF) ||   // 常用汉字
            (codepoint >= 0xFF00 && codepoint <= 0xFFEF) ||   // 全角字符
            (codepoint >= 0x3000 && codepoint <= 0x303F)) {   // CJK 符号
            width += 2;
            } else {
                width += 1;
            }
    }
    return width;
}

export enum Alignment {
    Left, Right, Middle
};


export class ConsoleDrawer {
    Int lineWidth;
public:
    explicit ConsoleDrawer(const Int width = 50) : lineWidth(width) {}

    void setLineWidth(const Int width) {
        lineWidth = width;
    }

    void drawHorizontalLine(const String& ch = "=") const {
        repeat(ch, lineWidth);
        cout << endl;
    }

    void drawBorderedLine(const String& content, Alignment align = Middle, const String& ch = "|") const {
        const Int bodyLineWidth = lineWidth - 2;
        const Int contentWidth = calcDisplayWidth(content);
        cout << ch;
        switch (align) {
            case Left:
                break;
            case Right:
                repeat(' ', bodyLineWidth - contentWidth);
                break;
            case Middle:
                repeat(' ', (bodyLineWidth - contentWidth) / 2);
                break;
        }
        cout << content;
        switch (align) {
            case Left:
                repeat(' ', bodyLineWidth - contentWidth);
                break;
            case Right:
                break;
            case Middle:
                repeat(' ', (bodyLineWidth - contentWidth) / 2 + (bodyLineWidth - contentWidth) % 2);
                break;
        }
        cout << ch;
        cout << endl;
    }

    void drawOption(Int index, const String& content, const String& ch = " ") const {
        cout << ch;
        cout << Stringf::format("[%d] %s\n", index, content);
    }

    void drawBorderedOption(Int index, const String& content, const String& ch = "|") const {
        drawBorderedLine(Stringf::format(" [%d] %s", index, content), Left, ch);
    }

    void drawTip(const String &content) const {
        cout << content << ": ";
    }

    void drawMessage(const String &content) const {
        cout << content << endl;
    }
};