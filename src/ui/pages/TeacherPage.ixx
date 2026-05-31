module;
#include <algorithm>
#include <iostream>
export module pages.TeacherPage;
import Format;
import Types;
import Logger;
import Page;
import PageState;
import UserPage;
import User;
import Book;
import Record;
import Runtime;
import Hash;
import Time;
import Config;

export class TeacherPage final : public UserPage {
    Int drawMainMenu() {
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine(Stringf::format("图书借还管理系统"));
        drawer.drawBorderedLine(Stringf::format("当前用户: %s, 教师", currentUser.getUserName()));
        drawer.drawHorizontalLine();

        drawer.drawBorderedOption(1, "借书");
        drawer.drawBorderedOption(2, "还书");
        drawer.drawBorderedOption(3, "列出所有书籍");
        drawer.drawBorderedOption(4, "查看热门排行");
        drawer.drawBorderedOption(5, "查看我的借阅");
        drawer.drawBorderedOption(6, "修改密码");
        drawer.drawBorderedOption(0, "退出登录");

        drawer.drawHorizontalLine("-");
        drawer.drawTip("选择操作");
        return numberInput();
    }
public:
    PageState draw() override {
        while (true) {
            clearScreen();
            switch (drawMainMenu()) {
                case 1: executeBorrow(std::stoi(Config::get("TeacherBorrowLimit")), true);break;
                case 2: executeReturn();            break;
                case 3: executeListAllBooks();      break;
                case 4: executeListHot();           break;
                case 5: executeCheckRecord();       break;
                case 6: executeModifyPassword();    break;
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
                    onErrorInput("0-6");
                    break;
            }
        }
    }
};