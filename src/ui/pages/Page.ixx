module;
#include <conio.h>
#include <iostream>
export module Page;
import PageState;
import Types;

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
        cout << "按任意键继续...";
        _getch();
        return page;
    }
protected:
    /**
     * @tparam T 请求类型
     * @return 到下一个空格为止的值
     */
    template<typename T>
    inline T requestInput() {
        T value;
        cin >> value;
        cin.clear();
        return value;
    }

    /**
     * @return 整行字符串
     */
    static inline String requestLine() {
        String line;
        getline(cin, line);
        return line;
    }
};