module;
#include <iostream>
export module pages.AdminPage;
import Format;
import Types;
import Logger;
import Page;
import PageState;
import User;
import Hash;
import Runtime;
import Book;
import Record;
import Time;
import Config;
import UserPage;

export class AdminPage final : public UserPage {
    [[nodiscard]] Int drawMainMenu() const {
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("图书借还管理系统 · 管理员后台");

        const String info = Stringf::format("当前馆藏: %d | 异常超期: %d",
            Book::getTotalBookCount(sqlite.get()),
            Record::getOverdueCount(0, sqlite.get()));
        drawer.drawBorderedLine(info);
        drawer.drawHorizontalLine();

        drawer.drawBorderedOption(1, "注册教师账户");
        drawer.drawBorderedOption(2, "注销教师账户");
        drawer.drawBorderedOption(3, "添加新书");
        drawer.drawBorderedOption(4, "删除书籍");
        drawer.drawBorderedOption(5, "列出所有用户");
        drawer.drawBorderedOption(6, "列出所有书籍");
        drawer.drawBorderedOption(7, "列出所有借阅记录");
        drawer.drawBorderedOption(8, "系统设置");
        drawer.drawBorderedOption(0, "注销并退出");
        drawer.drawHorizontalLine("-");
        drawer.drawTip("选择操作");
        return numberInput();
    }

    void executeRegisterTeacher() {
        start:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("注册教师账户");
        drawer.drawHorizontalLine();
        drawer.drawTip("请输入教师账户(按Esc返回)");

        const auto optUsername = readLineWithCancel();
        if (!optUsername.has_value()) return;
        const String& username = optUsername.value();

        if (User::contains(username, sqlite.get())) {
            drawer.drawMessage("该账户已存在，请重新输入！");
            wait();
            goto start;
        }

        inputPwd:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("注册教师账户");
        drawer.drawHorizontalLine();
        drawer.drawMessage(Stringf::format("注册教师: %s", username));
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

        User teacher{username, SHA256::hashr(firstOptPwd.value()), Teacher};
        teacher.store(sqlite.get());
        drawer.drawMessage(Stringf::format("教师账户 %s 注册成功！", username));
        wait();
    }

    void executeDeleteTeacher() {
        start:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("注销教师账户");
        drawer.drawHorizontalLine();
        drawer.drawTip("请输入要注销的教师账户(按Esc返回)");

        const auto optUsername = readLineWithCancel();
        if (!optUsername.has_value()) return;
        const String& username = optUsername.value();
        if (!User::contains(username, sqlite.get())) {
            drawer.drawMessage(Stringf::format("账户为 %s 的教师不存在！", username));
            wait();
            goto start;
        }

        auto overdueUserIds = Record::getOverdueUserIds(std::stol(Config::get("MaxBorrowDuration")), sqlite.get());
        if (overdueUserIds.contains(hash64(username))) {
            drawer.drawMessage("该账户还有未归还的书籍，请处理后再尝试删除！");
            wait();
            goto start;
        }

        drawer.drawTip(Stringf::format("确定要注销教师 %s 吗？此操作不可逆！(y/N)", username));

        if (boolInput()) {
            drawer.drawMessage(Stringf::format("\n已删除教师账户: %s", username));
            User::remove(username, sqlite.get());
            wait();
        }
        goto start;
    }

    void executeAddBook() {
        start:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("添加书籍");
        drawer.drawHorizontalLine();
        drawer.drawTip("请输入图书唯一编号 (按Esc返回)");

        auto optCode = readLineWithCancel();
        if (!optCode.has_value()) return;
        auto code = optCode.value();
        if (Book::contains(code, sqlite.get())) {
            drawer.drawMessage("该编号已存在，请重新输入！");
            wait();
            goto start;
        }

        drawer.drawTip("请输入书名(按Esc返回)");
        auto optTitle = readLineWithCancel();
        if (!optTitle.has_value()) goto start;

        drawer.drawMessage("请选择图书类型:");
        drawer.drawOption(1, "普通图书");
        drawer.drawOption(2, "珍贵图书");
        const Int typeOpt = numberInput();
        String type = typeOpt == 2 ? "PRECIOUS" : "REGULAR";

        Book b{optTitle.value(), code, static_cast<BookType>(typeOpt - 1), static_cast<BookStatus>(0)};
        b.store(sqlite.get());
        drawer.drawMessage(Stringf::format("图书 %s 添加成功！", optTitle.value()));
        wait();
        goto start;
    }

    void executeDeleteBook() {
        start:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("删除书籍");
        drawer.drawHorizontalLine();
        drawer.drawTip("请输入要删除的书籍唯一编号(按Esc返回)");

        const auto optBookCode = readLineWithCancel();
        if (!optBookCode.has_value()) return;
        const String& bookCode = optBookCode.value();
        if (!Book::contains(bookCode, sqlite.get())) {
            drawer.drawMessage(Stringf::format("编号为 %s 的书籍不存在！", bookCode));
            wait();
            goto start;
        }

        drawer.drawTip(Stringf::format("确定要删除书籍 %s 吗？此操作不可逆！(y/N)", bookCode));

        if (boolInput()) {
            drawer.drawMessage(Stringf::format("\n已删除书籍: %s", bookCode));
            Book::remove(bookCode, sqlite.get());
            wait();
        }
        goto start;
    }

    void executeListAllUsers() {
        clearScreen();
        const List<Long> ids = User::getAllUserIds(sqlite.get());
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine(Stringf::format("当前系统共 %d 名用户", ids.size()));
        drawer.drawHorizontalLine();

        drawer.drawDoubleMessage("用户名", "类型");
        drawer.drawHorizontalLine("-");
        for (const auto& id : ids) {
            const User* user = User::load(id, sqlite.get());
            drawer.drawDoubleMessage(user->getUserName(), typeToString(user->getType()));
            delete user;
        }
        drawer.drawHorizontalLine("-");
        wait();
    }

    void executeListAllRecords() {
        clearScreen();
        const List<Long> ids = Record::getAllRecordIds(sqlite.get());
        drawer.drawHorizontalLine();

        if (ids.size() == 0) {
            drawer.drawBorderedLine("目前没有借阅记录");
            drawer.drawHorizontalLine();
        }
        else {
            drawer.drawBorderedLine(Stringf::format("目前存储了 %d 次借阅记录", ids.size()));
            drawer.drawHorizontalLine();

            drawer.drawTripleMessage("书名", "用户", "时间");
            drawer.drawHorizontalLine("-");
            for (const auto& id : ids) {
                const Record* record = Record::loadById(id, sqlite.get());
                const User* user = User::load(record->getUserId(), sqlite.get());
                const Book* book = Book::load(record->getBookId(), sqlite.get());
                drawer.drawTripleMessage(book->getTitle(), user->getUserName(), formatTimeMillis(record->getBorrowDate()));
                delete record;
                delete user;
                delete book;
            }
            drawer.drawHorizontalLine("-");
        }
        wait();
    }

    void executeSystemSettings() {
        start:
        const auto studentLimit = Config::get("StudentBorrowLimit");
        const auto teacherLimit = Config::get("TeacherBorrowLimit");
        const auto dateLimit = Config::get("MaxBorrowDuration");
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("系统设置");
        drawer.drawHorizontalLine();

        drawer.drawBorderedOption(1, Stringf::format("修改学生借阅上限，当前值为 %s", studentLimit));
        drawer.drawBorderedOption(2, Stringf::format("修改教师借阅上限，当前值为 %s", teacherLimit));
        drawer.drawBorderedOption(3, Stringf::format("修改最大借阅时长，当前值为 %s", dateLimit));
        drawer.drawBorderedOption(-1, "返回");
        drawer.drawHorizontalLine("-");
        drawer.drawTip("请选择操作");

        Int choice = numberInput();
        drawer.clearLine();
        Boolean shouldWait = true;
        switch (choice) {
            case 1: {
                drawer.drawTip("请输入新的学生借阅上限册数(按Esc返回)");
                auto opt = readLineWithCancel();
                if (!opt.has_value()) {shouldWait = false; break;}
                const auto s = opt.value();
                Int limit;
                if (auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), limit); ec == std::errc()) {
                    drawer.drawMessage(Stringf::format("已修改学生借阅上限: %s -> %d", studentLimit, limit));
                    Config::set("StudentBorrowLimit", std::to_string(limit));
                } else if (ec == std::errc::invalid_argument) {
                    drawer.drawMessage("无效输入");
                } else if (ec == std::errc::result_out_of_range) {
                    drawer.drawMessage("输入太大了");
                }
                break;
            }
            case 2: {
                drawer.drawTip("请输入新的教师借阅上限册数(按Esc返回)");
                auto opt = readLineWithCancel();
                if (!opt.has_value()) {shouldWait = false; break;}
                const auto s = opt.value();
                Int limit;
                if (auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), limit); ec == std::errc()) {
                    drawer.drawMessage(Stringf::format("已修改教师借阅上限: %s -> %d", teacherLimit, limit));
                    Config::set("TeacherBorrowLimit", std::to_string(limit));
                } else if (ec == std::errc::invalid_argument) {
                    drawer.drawMessage("无效输入");
                } else if (ec == std::errc::result_out_of_range) {
                    drawer.drawMessage("输入太大了");
                }
                break;
            }
            case 3: {
                drawer.drawTip("请输入新的借阅时长限制(按Esc返回)");
                auto opt = readLineWithCancel();
                if (!opt.has_value()) {shouldWait = false; break;}
                const auto s = opt.value();
                Long limit;
                if (auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), limit); ec == std::errc()) {
                    drawer.drawMessage(Stringf::format("已修改借阅时长上限: %s -> %d", dateLimit, static_cast<long long>(limit)));
                    Config::set("MaxBorrowDuration", std::to_string(limit));
                } else if (ec == std::errc::invalid_argument) {
                    drawer.drawMessage("无效输入");
                } else if (ec == std::errc::result_out_of_range) {
                    drawer.drawMessage("输入太大了");
                }
                break;
            }
            default:
                return;
        }
        if (shouldWait) wait();
        goto start;
    }

public:
    AdminPage() = default;

    PageState draw() override {
        while (true) {
            switch (drawMainMenu()) {
                case 1: executeRegisterTeacher(); break;
                case 2: executeDeleteTeacher();   break;
                case 3: executeAddBook();         break;
                case 4: executeDeleteBook();      break;
                case 5: executeListAllUsers();    break;
                case 6: executeListAllBooks();    break;
                case 7: executeListAllRecords();  break;
                case 8: executeSystemSettings();  break;
                case 0:
                    clearScreen();
                    drawer.drawTip("确定要注销管理员登录吗?(y/N)");
                    if (boolInput()) {
                        drawer.drawMessage("\n已注销管理员登录");
                        wait();
                        // currentAccountType = None;
                        currentUser = User::none();
                        return PageState::Welcome;
                    }
                    break;
                default:
                    onErrorInput("0-8");
                    break;
            }
        }
    }
};