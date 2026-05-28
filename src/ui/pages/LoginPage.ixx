module;
#include <iostream>
export module pages.LoginPage;
import Format;
import Types;
import Logger;
import Page;
import PageState;
import ConsoleDrawer;
import User;

enum class LoginState {
    Success, PasswordError, Back
};

export class LoginPage final : public Page {
    LoginState inPasswordPage(const String& username) {
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("用户登录");
        drawer.drawHorizontalLine();
        drawer.drawMessage(Stringf::format("用户 %s, 您好!", username));
        drawer.drawHorizontalLine("-");
        drawer.drawTip("请输入密码(按Esc返回)");
        auto optPwd = readLineWithCancel(true);
        if (!optPwd.has_value()) return LoginState::Back;

        auto pwd = optPwd.value();
        if (pwd == "123456") return LoginState::Success;

        return LoginState::PasswordError;
    }

     std::optional<String> inUserNamePage() {
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("用户登录");
        drawer.drawHorizontalLine();
        drawer.drawTip("请输入用户名(按Esc返回)");
        return readLineWithCancel();
    }

public:
    LoginPage() = default;

    PageState draw() override {
        start:
        auto optUsername = inUserNamePage();
        if (!optUsername.has_value()) {
            return PageState::Welcome;
        }
        String username = optUsername.value();
        while (true) {
            clearScreen();
            switch (inPasswordPage(username)) {
                case LoginState::Success:
                    drawer.drawMessage("登录成功");
                    wait();

                    if (username == "admin") {
                        currentAccountType = Admin;
                        return PageState::AdminMenu;
                    }
                    return PageState::Welcome;
                case LoginState::PasswordError:
                    drawer.drawMessage("密码错误，请重新输入!");
                    wait();
                    break;
                case LoginState::Back:
                    clearScreen();
                    goto start;
            }
        }
    }
};