module;
#include <iostream>
export module pages.WelcomePage;
import Format;
import Types;
import Logger;
import Page;
import PageState;

export class WelcomePage final : public Page {
public:
    PageState draw() override {
        start:
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("欢迎使用 图书借还管理系统");
        drawer.drawHorizontalLine();
        drawer.drawBorderedOption(1, "用户登录");
        drawer.drawBorderedOption(2, "学生注册");
        drawer.drawBorderedOption(0, "退出系统");
        drawer.drawHorizontalLine("-");
        drawer.drawTip("选择操作");
        while (true) {
            const Int option = numberInput();
            cout << option << '\n';
            switch (option) {
                case 1: return PageState::Login;
                case 2: return PageState::Register;
                case 0: return PageState::Exit;
                default:
                    onErrorInput("0-2");
                    clearScreen();
                    goto start;
            }
        }
    }
};