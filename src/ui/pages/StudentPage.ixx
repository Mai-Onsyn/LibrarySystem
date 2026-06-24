module;
#include <iostream>
export module pages.StudentPage;
import Format;
import Types;
import Page;
import PageState;
import UserPage;
import User;
import Config;
import Hash;
import Runtime;

export class StudentPage final : public UserPage {
    Int drawMainMenu() {
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine(Stringf::format("图书借还管理系统"));
        drawer.drawBorderedLine(Stringf::format("当前用户: %s, 学生", currentUser.getUserName()));
        drawer.drawHorizontalLine();

        drawer.drawBorderedOption(1, "借书");
        drawer.drawBorderedOption(2, "还书");
        drawer.drawBorderedOption(3, "列出所有书籍");
        drawer.drawBorderedOption(4, "查看热门排行");
        drawer.drawBorderedOption(5, "查看我的借阅");
        drawer.drawBorderedOption(6, "修改密码");
        drawer.drawBorderedOption(7, "删除账户");
        drawer.drawBorderedOption(0, "退出登录");

        drawer.drawHorizontalLine("-");
        drawer.drawTip("选择操作");
        return numberInput();
    }

    Boolean executeDeleteAccount() {
        String username = currentUser.getUserName();
        verify:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("验证使用者");
        drawer.drawHorizontalLine();
        drawer.drawTip("请输入密码(按Esc返回)");
        auto optPwd = readLineWithCancel(true);
        if (!optPwd.has_value()) return false;

        if (SHA256::hashr(optPwd.value()) != currentUser.getPasswordHashes()) {
            drawer.drawMessage("密码错误，请重新输入");
            wait();
            goto verify;
        }

        drawer.drawTip(Stringf::format("真的要删除账户 %s 吗？此操作不可逆！(y/N)", username));
        if (boolInput()) {
            drawer.drawTip(Stringf::format("\n请输入\"%s\"来确认执行删除操作", username));
            auto optUsername = readLineWithCancel();
            if (!optUsername.has_value()) goto verify;
            if (optUsername.value() == username) {
                User::remove(username, sqlite.get());
                drawer.drawMessage("账户已删除");
                currentUser = User::none();
                wait();
                return true;
            } else {
                drawer.drawMessage("验证错误，请重试");
                wait();
                goto verify;
            }
        }
        else goto verify;
    }
public:
    PageState draw() override {
        while (true) {
            clearScreen();
            switch (drawMainMenu()) {
                case 1: executeBorrow(std::stoi(Config::get("StudentBorrowLimit")), false);break;
                case 2: executeReturn();            break;
                case 3: executeListAllBooks();      break;
                case 4: executeListHot();           break;
                case 5: executeCheckRecord();       break;
                case 6: executeModifyPassword();    break;
                case 7: {
                    if (executeDeleteAccount()) return PageState::Welcome;
                    break;
                }
                case 0:
                    clearScreen();
                    drawer.drawTip("确定要注销登录吗?(y/N)");
                    if (boolInput()) {
                        drawer.drawMessage("\n已注销用户登录");
                        wait();
                        currentUser = User::none();
                        return PageState::Welcome;
                    }
                    break;
                default:
                    onErrorInput("0-7");
                    break;
            }
        }
    }
};