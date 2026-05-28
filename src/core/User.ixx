module;
export module User;
import Types;
import Storable;
import SqliteConnection;

export enum UserType {
    None,
    Teacher,
    Student,
    Admin
};

export UserType currentAccountType = None;

export class User final : public Storable {
    String username;
    String pwdHashes;
    UserType type;
public:
    User(const String& username, const String& pwdHashes, const UserType type) : username(username), pwdHashes(pwdHashes), type(type) {}

    void store(SqliteConnection *connection) override {

    }

    User* load(SqliteConnection *connection) override {
        return nullptr;
    }
};