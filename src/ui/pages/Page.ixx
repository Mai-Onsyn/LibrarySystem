module;
#include <conio.h>
#include <iostream>
export module Page;
import PageState;
import Types;
import ConsoleDrawer;

export class Page {
public:
    Page() = default;
    virtual ~Page() = default;

    /**
     * @return 由该页面决定的下一个页面
     */
    virtual PageState draw() = 0;

    PageState enter() {
        const auto page = draw();
        // wait();
        return page;
    }

    static inline void clearScreen() {
#if _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
protected:
    ConsoleDrawer drawer{};

    /**
     * @tparam T 请求类型
     * @return 到下一个空格为止的值
     */
    template<typename T>
    static inline T requestInput() {
        T value;
        cin >> value;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return value;
    }

    /**
     * 快速获取一个数字输入 用于菜单选择
     * @return 数字0~9 若输入不是数字 返回-1
     */
    static inline Int numberInput() {
        if (const Int ch = _getch(); ch >= '0' && ch <= '9') {
            return ch - '0';
        }
        return -1;
    }

    /**
     * 快速获取一个y/n输入
     * y/Y -> true
     * 其他字符 -> false
     * @return true / false
     */
    static inline Boolean boolInput() {
        if (const Int ch = _getch(); ch == 'y' || ch == 'Y') {
            return true;
        }
        return false;
    }

    /**
     * @return 整行字符串
     */
    static inline String requestLine() {
        cin >> std::ws;
        String line;
        getline(cin, line);
        return line;
    }

    std::optional<String> readLineWithCancel(const Boolean password = false) {
        String input;
        char ch;

        while (true) {
            ch = _getch();

            // ESC
            if (ch == 27 || ch == 9) {
                return std::nullopt;
            }

            // Enter
            if (ch == 13) {
                cout << std::endl;
                return input;
            }

            // BackScape
            if (ch == 8) {
                if (!input.empty()) {
                    input.pop_back();
                    cout << "\b \b" << std::flush;
                }
                continue;
            }

            // 一般字符
            if (ch >= 32 && ch <= 126) {
                input.push_back(ch);
                if (password) cout << '*' << std::flush;
                else cout << ch << std::flush;
            }
        }
    }

    static inline void wait() {
        cout << "按任意键继续...";
        _getch();
    }

    static inline void onErrorInput(const String& reason = "") {
        cout << "输入错误，请重新输入" << (reason.empty() ? "" : " (" + reason + ")") << '\n';
        wait();
    }
};