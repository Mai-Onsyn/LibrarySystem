module;
#include <iostream>
export module pages.RegisterPage;
import Format;
import Types;
import Page;
import PageState;
import ConsoleDrawer;
import User;
import Runtime;
import Hash;

export class RegisterPage final : public Page {
public:
    RegisterPage() = default;

    PageState draw() override {
        start:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("学生注册");
        drawer.drawHorizontalLine();
        drawer.drawTip("请输入用户名(按Esc返回)");

        const auto optUsername = readLineWithCancel();
        if (!optUsername.has_value()) {
            return PageState::Welcome;
        }
        const String& username = optUsername.value();

        if (User::contains(username, sqlite.get())) {
            drawer.drawMessage("该账户已存在，请重新输入！");
            wait();
            goto start;
        }

        inputPwd:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("学生注册");
        drawer.drawHorizontalLine();
        drawer.drawMessage(Stringf::format("注册用户: %s", username));
        drawer.drawHorizontalLine("-");
        drawer.drawTip("请设置登录密码(按Esc返回)");

        auto firstOptPwd = readLineWithCancel(true);
        if (!firstOptPwd.has_value()) goto start;
        const String& firstPwd = firstOptPwd.value();

        drawer.drawTip("请确认输入的密码(按Esc返回)");
        auto secondOptPwd = readLineWithCancel(true);
        if (!secondOptPwd.has_value()) goto inputPwd;

        const String& secondPwd = secondOptPwd.value();
        if (firstPwd != secondPwd) {
            drawer.drawMessage("两次输入的密码不一致，请重新输入！");
            wait();
            goto inputPwd;
        }

        // 创建学生用户并哈希加密密码
        User student{username, SHA256::hashr(firstPwd), Student};
        student.store(sqlite.get());

        drawer.drawMessage(Stringf::format("学生账户 %s 注册成功！请登录。", username));
        wait();

        return PageState::Welcome;
    }
};