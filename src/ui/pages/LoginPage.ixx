module;
#include <iostream>
export module pages.LoginPage;
import Format;
import Types;
import Page;
import PageState;
import ConsoleDrawer;
import User;
import Runtime;
import Hash;

enum class LoginState {
    Success, PasswordError, Back
};

export class LoginPage final : public Page {
    LoginState inPasswordPage(const User* user) {
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("用户登录");
        drawer.drawHorizontalLine();
        drawer.drawMessage(Stringf::format("用户 %s, 您好!", user->getUserName()));
        drawer.drawHorizontalLine("-");
        drawer.drawTip("请输入密码(按Esc返回)");
        auto optPwd = readLineWithCancel(true);
        if (!optPwd.has_value()) return LoginState::Back;

        if (SHA256::hashr(optPwd.value()) == user->getPasswordHashes()) return LoginState::Success;
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
        String username;
        while (true) {
            auto optUsername = inUserNamePage();
            if (!optUsername.has_value()) {
                return PageState::Welcome;
            }
            username = optUsername.value();
            if (User::contains(username, sqlite.get())) {
                break;
            }
            drawer.drawMessage(Stringf::format("用户 %s 不存在!", username));
            wait();
            clearScreen();
        }
        UniquePtr<User> user(User::load(username, sqlite.get()));
        while (true) {
            clearScreen();
            switch (inPasswordPage(user.get())) {
                case LoginState::Success:
                    drawer.drawMessage("登录成功");
                    wait();

                    currentUser = *user.get();
                    switch (user.get()->getType()) {
                        case Admin:
                            return PageState::AdminMenu;
                        case Student:
                            return PageState::StudentMenu;
                        case Teacher:
                            return PageState::TeacherMenu;
                        default:
                            drawer.drawMessage("发生了未知错误");
                            return PageState::Welcome;
                    }
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