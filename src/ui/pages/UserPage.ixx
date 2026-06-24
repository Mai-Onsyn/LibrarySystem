module;
#include <algorithm>
#include <string>
#include <vector>
export module UserPage;
import Page;
import User;
import ConsoleDrawer;
import Types;
import PageState;
import Book;
import Runtime;
import Format;
import Time;
import Hash;
import Record;
import Config;

String bookStatusToString(const BookStatus s) {
    switch (s) {
        case Available: return "可借";
        case Borrowed: return "已借出";
    }
    return "错误";
}

export class UserPage : public Page {
protected:
    void executeListAllBooks() {
        clearScreen();
        const List<Long> ids = Book::getAllBookIds(sqlite.get());
        drawer.drawHorizontalLine();
        if (ids.empty()) {
            drawer.drawBorderedLine("当前系统没有书籍");
            drawer.drawHorizontalLine();
        }
        else {
            drawer.drawBorderedLine(Stringf::format("当前系统共 %d 本书", ids.size()));
            drawer.drawHorizontalLine();

            drawer.drawTripleMessage("编号", "标题", "状态");
            drawer.drawHorizontalLine("-");
            for (const auto& id : ids) {
                const Book* book = Book::load(id, sqlite.get());
                drawer.drawTripleMessage(book->getCode(), book->getTitle(), bookStatusToString(book->getStatus()));
                delete book;
            }
            drawer.drawHorizontalLine("-");
        }
        wait();
    }

    void executeBorrow(const Int limit, const Boolean permission) {
        start:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("借书");
        drawer.drawHorizontalLine();

        List<Long> userBorrowIds = Record::getUserShouldReturnRecordIds(currentUser.getUserId(), sqlite.get());
        if (userBorrowIds.size() >= limit) {
            drawer.drawMessage(Stringf::format("您的借阅数量已达到上限(%d本)", limit));
            drawer.drawMessage("请先归还部分书籍之后再尝试借阅！");
            wait();
            return;
        }

        drawer.drawTip("请输入要借的书籍编号(按Esc取消)");

        auto optCode = readLineWithCancel();
        if (!optCode.has_value()) return;

        const String& code = optCode.value();
        UniquePtr<Book> book(Book::load(code, sqlite.get()));
        if (!book.get()) {
            drawer.drawMessage("该编号不存在，请重试！");
            wait();
            goto start;
        }

        if (book->getStatus() != Available) {
            drawer.drawMessage("很抱歉，该书籍已被借出！");
            wait();
            goto start;
        }

        if (book->getType() == Precious && !permission) {
            drawer.drawMessage("很抱歉，您没有权限借阅贵重书籍！");
            wait();
            goto start;
        }

        drawer.drawTip(Stringf::format("确认要借这本 %d 吗?(y/N)", book.get()->getTitle()));
        if (boolInput()) {
            const Long now = millisTime();
            Record record{currentUser.getUserId(), hash64(book->getCode()), now};
            record.store(sqlite.get());
            book->setStatus(Borrowed);
            book->borrowOnce();
            book->store(sqlite.get());
            const Long duration =  std::stoll(Config::get("MaxBorrowDuration"));
            drawer.drawMessage(Stringf::format("\n借书成功，请在%s前及时归还！", formatTimeMillis(now + duration)));
            wait();
        }
        goto start;
    }

    void executeReturn() {
        start:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("还书");
        drawer.drawHorizontalLine();
        auto records = Record::getUserShouldReturnRecordIds(currentUser.getUserId(), sqlite.get());
        if (records.isEmpty()) {
            drawer.drawMessage("您目前没有需要归还的书");
            wait();
            return;
        }
        drawer.drawBorderedLine(Stringf::format("这是您目前正在借阅的书(%d本)", records.size()));
        drawer.drawTripleMessage("编号", "书名", "借书时间");
        List<std::pair<String, UniquePtr<Record>>> recordElements{};
        for (const auto &recordId : records) {
            UniquePtr<Record> record(Record::loadById(recordId, sqlite.get()));
            auto [code, title] = record->getBookCodeAndTitle(sqlite.get());
            drawer.drawTripleMessage(code, title, formatTimeMillis(record->getBorrowDate(), "yy-MM-dd HH:mm"));
            recordElements.add(std::pair{code, std::move(record)});
        }
        drawer.drawHorizontalLine("-");
        drawer.drawTip("请输入要还的书籍编号(按Esc取消)");

        auto optCode = readLineWithCancel();
        if (!optCode.has_value()) return;
        const auto& code = optCode.value();

        Record* targetRecord = nullptr;
        for (const auto&[codeStr, recordPtr] : recordElements) {
            if (code == codeStr) {
                targetRecord = recordPtr.get();
                break;
            }
        }

        if (targetRecord == nullptr) {
            drawer.drawTip("您没有借阅这本书，请重新输入");
            wait();
            goto start;
        }

        drawer.drawTip(Stringf::format("确定要归还这本 %s 吗？(y/N)", code));
        if (boolInput()) {
            UniquePtr<Book> book(Book::load(code, sqlite.get()));
            if (book.get()) {
                book->setStatus(Available);
                book->store(sqlite.get());

                targetRecord->setReturnDate(millisTime());
                targetRecord->store(sqlite.get());

                drawer.drawMessage("\n还书成功！");
            } else {
                drawer.drawMessage("\n系统错误：未能找到该书籍数据！");
            }
            wait();
        }

        goto start;
    }

    void executeListHot() {
        List<Long> hotIds = Book::getHotIds(10, sqlite.get());
        clearScreen();

        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("热门书籍");
        drawer.drawHorizontalLine();

        drawer.drawTripleMessage("书名", "编号", "借阅人次");
        for (const Long id : hotIds) {
            const Book* book = Book::load(id, sqlite.get());
            drawer.drawTripleMessage(book->getTitle(), book->getCode(), std::to_string(book->getBorrowCount()));
            delete book;
        }
        drawer.drawHorizontalLine("-");

        wait();
    }

    void executeCheckRecord() {
        List<Long> recordIds = Record::getUserRecordIds(currentUser.getUserId(), sqlite.get());

        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine(Stringf::format("借阅记录(%d次)", recordIds.size()));
        drawer.drawHorizontalLine();

        if (recordIds.isEmpty()) {
            drawer.drawMessage("您没有借阅过任何书籍");
            wait();
            return;
        }

        drawer.draw4ColumnMessage("书籍编号", "书名", "借阅日期", "状态");
        List<std::pair<Long, UniquePtr<Record>>> timeRecordList;
        for (const auto id : recordIds) {
            UniquePtr<Record> record(Record::loadById(id, sqlite.get()));
            timeRecordList.add(std::pair{record->getBorrowDate(), std::move(record)});
        }
        std::ranges::sort(timeRecordList,
                          [](const auto& a, const auto& b) {
                              return a.first < b.first;
                          });
        for (const auto& [time, ptr] : timeRecordList) {
            auto [code, title] = ptr->getBookCodeAndTitle(sqlite.get());
            drawer.draw4ColumnMessage(code, title, formatTimeMillis(time), ptr->getReturnDate() == 0 ? "未归还" : "已归还");
        }

        drawer.drawHorizontalLine("-");
        wait();
    }

    void executeModifyPassword() {
        String username = currentUser.getUserName();
        verify:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("验证使用者");
        drawer.drawHorizontalLine();
        drawer.drawTip("请输入密码(按Esc返回)");
        auto optPwd = readLineWithCancel(true);
        if (!optPwd.has_value()) return;

        if (SHA256::hashr(optPwd.value()) != currentUser.getPasswordHashes()) {
            drawer.drawMessage("密码错误，请重新输入");
            wait();
            goto verify;
        }
        inputPwd:
        clearScreen();
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("修改密码");
        drawer.drawHorizontalLine();
        drawer.drawTip("请设置新的登录密码(按Esc返回)");
        auto firstOptPwd = readLineWithCancel(true);
        if (!firstOptPwd.has_value()) goto verify;
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

        User user{username, SHA256::hashr(firstOptPwd.value()), currentUser.getType()};
        user.store(sqlite.get());
        currentUser = user;
        drawer.drawMessage(Stringf::format("已成功修改账户密码！"));
        wait();
    }
};
