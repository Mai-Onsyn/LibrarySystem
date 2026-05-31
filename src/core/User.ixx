module;
#include <string>
export module User;
import Types;
import Storable;
import SqliteConnection;
import Hash;
import Time;
import Format;

export enum UserType {
    None,
    Teacher,
    Student,
    Admin
};

export String typeToString(const UserType type) {
    switch (type) {
        case Teacher: return "Teacher";
        case Student: return "Student";
        case Admin: return "Admin";
        default: return "Unknown";
    }
}

UserType stringToType(const String& type) {
    if (type == "Teacher") return Teacher;
    if (type == "Student") return Student;
    if (type == "Admin") return Admin;
    return None;
}

// export UserType currentAccountType = None;

export class User final : public Storable {
    String username;
    String pwdHashes;
    UserType type;
    Long createDate;
public:
    User(const String& username, const String& pwdHashes, const UserType type, const Long createDate = millisTime()):
        username(username), pwdHashes(pwdHashes), type(type), createDate(createDate) {}

    [[nodiscard]] String getUserName() const {
        return username;
    }

    [[nodiscard]] String getPasswordHashes() const {
        return pwdHashes;
    }

    [[nodiscard]] UserType getType() const {
        return type;
    }

    [[nodiscard]] Long getCreateDate() const {
        return createDate;
    }

    [[nodiscard]] Long getUserId() const {
        return hash64(username);
    }

    void store(const SqliteConnection *connection) override {
        connection->executeUpdate(R"(
            INSERT INTO Users (id, username, password, role, create_date)
            VALUES (?, ?, ?, ?, ?)
            ON CONFLICT(id) DO UPDATE SET
                username = excluded.username,
                password = excluded.password,
                role = excluded.role,
                create_date = excluded.create_date;
            )",
            hash64(username), username, pwdHashes, typeToString(type), createDate
        );
    }

    static User* load(const String& username, const SqliteConnection *connection) {
        if (!contains(username, connection)) return nullptr;

        return load(hash64(username), connection);
    }

    static User* load(const Long id, const SqliteConnection *connection) {
        const String qName = connection->query<String>("SELECT username FROM Users WHERE id = ?", 0, id)[0];
        const String qPwd = connection->query<String>("SELECT password FROM Users WHERE id = ?", 0, id)[0];
        const String qType = connection->query<String>("SELECT role FROM Users WHERE id = ?", 0, id)[0];
        const Long createDate = connection->query<Long>("SELECT create_date FROM Users WHERE id = ?", 0, id)[0];
        return new User(qName, qPwd, stringToType(qType), createDate);
    }

    static Boolean contains(const String& username, const SqliteConnection* connection) {
        return connection->containsID("Users", hash64(username));
    }

    static void removeById(Long id, const SqliteConnection* connection) {
        connection->executeUpdate("DELETE FROM Users WHERE id = ?", id);
    }

    static void remove(const String& username, const SqliteConnection* connection) {
        removeById(hash64(username), connection);
    }

    static List<Long> getAllUserIds(const SqliteConnection* connection) {
        return connection->query<Long>("SELECT id FROM Users", 0);
    }

    String toString() {
        return Stringf::format("User{name=%s, type=%s, createDate=%s}", username, typeToString(type), formatTimeMillis(createDate));
    }

    static User none() {
        return User{"", "", None, 0};
    }
};

export User currentUser = User::none();