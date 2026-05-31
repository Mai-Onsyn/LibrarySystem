module;
#include <string>
export module Book;
import Types;
import Storable;
import SqliteConnection;
import Hash;
import Format;

export enum BookType {
    Regular,
    Precious
};

export enum BookStatus {
    Available,
    Borrowed
};

String bookStatusToString(const BookStatus status) {
    switch (status) {
        case Available: return "Available";
        case Borrowed:  return "Borrowed";
        default:        return "Unknown";
    }
}

BookStatus stringToBookStatus(const String& status) {
    if (status == "Available") return Available;
    if (status == "Borrowed")  return Borrowed;
    return Available;
}

export class Book final : public Storable {
    String title;
    String code;
    BookType type;
    BookStatus status;
    Int borrowCount;

public:
    Book(const String& title, const String& code, const BookType type, const BookStatus status, const int borrowCount = 0)
        : title(title), code(code), type(type), status(status), borrowCount(borrowCount) {}

    [[nodiscard]] String getTitle() const { return title; }
    [[nodiscard]] String getCode()  const { return code; }
    [[nodiscard]] BookType getType() const { return type; }
    [[nodiscard]] BookStatus getStatus() const { return status; }
    [[nodiscard]] Int getBorrowCount() const { return borrowCount; }

    void setStatus(const BookStatus newStatus) { status = newStatus; }
    void incBorrowCount() { ++borrowCount; }

    void store(const SqliteConnection* connection) override {
        Long id = hash64(code);
        connection->executeUpdate(R"(
            INSERT INTO Books (id, title, code, type, status, borrow_count)
            VALUES (?, ?, ?, ?, ?, ?)
            ON CONFLICT(id) DO UPDATE SET
                title = excluded.title,
                code = excluded.code,
                type = excluded.type,
                status = excluded.status,
                borrow_count = excluded.borrow_count;
        )", id, title, code, static_cast<int>(type), bookStatusToString(status), borrowCount);
    }

    void borrowOnce() {
        borrowCount++;
    }

    static Book* load(const String& code, const SqliteConnection* connection) {
        if (!contains(code, connection)) return nullptr;

        const Long id = hash64(code);
        return load(id, connection);
    }

    static Book* load(Long id, const SqliteConnection* connection) {
        const String qTitle = connection->query<String>("SELECT title FROM Books WHERE id = ?", 0, id)[0];
        const String qCode = connection->query<String>("SELECT code FROM Books WHERE id = ?", 0, id)[0];
        const Int qType = connection->query<Int>("SELECT type FROM Books WHERE id = ?", 0, id)[0];
        const String qStatus = connection->query<String>("SELECT status FROM Books WHERE id = ?", 0, id)[0];
        const Int qBorrowCnt = connection->query<Int>("SELECT borrow_count FROM Books WHERE id = ?", 0, id)[0];

        return new Book(qTitle, qCode, static_cast<BookType>(qType), stringToBookStatus(qStatus), qBorrowCnt);
    }

    static Boolean contains(const String& code, const SqliteConnection* connection) {
        return connection->containsID("Books", hash64(code));
    }

    static void remove(const String& code, const SqliteConnection* connection) {
        connection->executeUpdate("DELETE FROM Books WHERE code = ?", code);
    }

    static void removeById(Long id, const SqliteConnection* connection) {
        connection->executeUpdate("DELETE FROM Books WHERE id = ?", id);
    }

    static Int getTotalBookCount(const SqliteConnection* connection) {
        return connection->query<Int>("SELECT COUNT(*) FROM Books", 0)[0];
    }

    static List<Long> getAllBookIds(const SqliteConnection* connection) {
        return connection->query<Long>("SELECT id FROM Books", 0);
    }

    static List<Long> getHotIds(const Int limit, const SqliteConnection* connection) {
        return connection->query<Long>("SELECT id FROM Books ORDER BY borrow_count DESC LIMIT ?", 0, limit);
    }

    [[nodiscard]] String toString() const {
        return Stringf::format("Book{code=%s, title=%s, type=%d, status=%s, borrowCount=%d}",
            code, title, static_cast<Int>(type), bookStatusToString(status), borrowCount);
    }
};