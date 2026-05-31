module;
#include <string>
export module Record;

import Types;
import Storable;
import SqliteConnection;
import Hash;
import Format;
import Time;
import Book;

export class Record final : public Storable {
    Long userId;
    Long bookId;
    Long borrowDate;
    Long returnDate;

    static Long makeId(Long userId, Long bookId, Long borrowDate) {
        return hash64(Stringf::format("%d:%d:%d", userId, bookId, borrowDate));
    }

public:
    Record(const Long userId, const Long bookId, const Long borrowDate, const Long returnDate = 0)
        : userId(userId), bookId(bookId), borrowDate(borrowDate), returnDate(returnDate) {}

    [[nodiscard]] Long getUserId() const { return userId; }
    [[nodiscard]] Long getBookId() const { return bookId; }
    [[nodiscard]] Long getBorrowDate() const { return borrowDate; }
    [[nodiscard]] Long getReturnDate() const { return returnDate; }

    void setReturnDate(const Long date) { returnDate = date; }
    [[nodiscard]] bool isReturned() const { return returnDate != 0; }

    void store(const SqliteConnection* connection) override {
        Long id = makeId(userId, bookId, borrowDate);
        connection->executeUpdate(R"(
                INSERT INTO BorrowRecord (id, user_id, book_id, borrow_date, return_date)
                VALUES (?, ?, ?, ?, ?)
                ON CONFLICT(id) DO UPDATE SET
                    user_id = excluded.user_id,
                    book_id = excluded.book_id,
                    borrow_date = excluded.borrow_date,
                    return_date = excluded.return_date;
            )",
        id, userId, bookId, borrowDate, returnDate);
    }

    std::pair<String, String> getBookCodeAndTitle(const SqliteConnection* connection) const {
        const Book* book = Book::load(bookId, connection);
        String code = book->getCode();
        String title = book->getTitle();
        delete book;
        return {code, title};
    }

    static Record* load(const Long userId, const Long bookId, const Long borrowDate, const SqliteConnection* connection) {
        Long id = makeId(userId, bookId, borrowDate);
        if (!contains(id, connection)) return nullptr;

        const Long qUserId = connection->query<Long>("SELECT user_id FROM BorrowRecord WHERE id = ?", 0, id)[0];
        const Long qBookId = connection->query<Long>("SELECT book_id FROM BorrowRecord WHERE id = ?", 0, id)[0];
        const Long qBorrow  = connection->query<Long>("SELECT borrow_date FROM BorrowRecord WHERE id = ?", 0, id)[0];
        const Long qReturn  = connection->query<Long>("SELECT return_date FROM BorrowRecord WHERE id = ?", 0, id)[0];

        return new Record(qUserId, qBookId, qBorrow, qReturn);
    }

    static Record* loadById(Long id, const SqliteConnection* connection) {
        if (!contains(id, connection)) return nullptr;

        const Long qUserId = connection->query<Long>("SELECT user_id FROM BorrowRecord WHERE id = ?", 0, id)[0];
        const Long qBookId = connection->query<Long>("SELECT book_id FROM BorrowRecord WHERE id = ?", 0, id)[0];
        const Long qBorrow  = connection->query<Long>("SELECT borrow_date FROM BorrowRecord WHERE id = ?", 0, id)[0];
        const Long qReturn  = connection->query<Long>("SELECT return_date FROM BorrowRecord WHERE id = ?", 0, id)[0];

        return new Record(qUserId, qBookId, qBorrow, qReturn);
    }

    static Boolean contains(const Long userId, const Long bookId, const Long borrowDate, const SqliteConnection* connection) {
        return contains(makeId(userId, bookId, borrowDate), connection);
    }

    static Boolean contains(const Long id, const SqliteConnection* connection) {
        return connection->containsID("BorrowRecord", id);
    }

    static void removeById(Long id, const SqliteConnection* connection) {
        connection->executeUpdate("DELETE FROM BorrowRecord WHERE id = ?", id);
    }

    static void remove(const Long userId, const Long bookId, const Long borrowDate, const SqliteConnection* connection) {
        connection->executeUpdate("DELETE FROM BorrowRecord WHERE id = ?", makeId(userId, bookId, borrowDate));
    }

    static int getOverdueCount(const Long overdueMillis, const SqliteConnection* connection) {
        Long threshold = millisTime() - overdueMillis;
        return connection->query<int>(
            "SELECT COUNT(*) FROM BorrowRecord WHERE return_date = 0 AND borrow_date <= ?",
            0, threshold)[0];
    }

    static List<Long> getAllRecordIds(const SqliteConnection* connection) {
        return connection->query<Long>("SELECT id FROM BorrowRecord", 0);
    }

    static List<Long> getOverdueUserIds(const Long overdueMillis, const SqliteConnection* connection) {
        Long threshold = millisTime() - overdueMillis;
        return connection->query<Long>(
            "SELECT DISTINCT user_id FROM BorrowRecord "
            "WHERE return_date = 0 AND borrow_date <= ?",
            0, threshold);
    }

    static List<Long> getUserRecordIds(const Long userId, const SqliteConnection* connection) {
        return connection->query<Long>("SELECT id FROM BorrowRecord WHERE user_id = ?", 0, userId);
    }

    static List<Long> getUserShouldReturnRecordIds(const Long userId, const SqliteConnection* connection) {
        return connection->query<Long>("SELECT id FROM BorrowRecord WHERE user_id = ? AND return_date = 0", 0, userId);
    }

    [[nodiscard]] String toString() const {
        String borrowStr = formatTimeMillis(borrowDate);
        String returnStr = returnDate == 0 ? "Not returned" : formatTimeMillis(returnDate);
        return Stringf::format("Record{user=%d, book=%d, borrow=%s, return=%s}",
            userId, bookId, borrowStr, returnStr);
    }
};