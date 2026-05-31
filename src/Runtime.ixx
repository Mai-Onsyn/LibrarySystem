module;
export module Runtime;
import SqliteConnection;
import Types;
import User;
import Hash;
import Config;

export void initSql(const SqliteConnection* connection) {
    connection->execute(R"(create table if not exists Users (
            id integer primary key autoincrement,
            username text not null,
            password text not null,
            role integer not null,
            create_date integer not null
        ))"
    );
    connection->execute(R"(create table if not exists Books (
            id integer primary key autoincrement,
            title text not null,
            code text not null,
            type integer not null,
            status text not null,
            borrow_count integer not null
        ))"
    );
    connection->execute(R"(create table if not exists BorrowRecord (
            id integer primary key autoincrement,
            user_id integer not null,
            book_id integer not null,
            borrow_date integer not null,
            return_date integer not null,
            foreign key (user_id) references Users(id) on delete cascade,
            foreign key (book_id) references Books(id) on delete cascade
        ))"
    );
    connection->execute(R"(create table if not exists Config (
            id integer primary key autoincrement,
            key text not null,
            value text not null
        ))"
    );
    User admin{"admin", SHA256::hashr("114514"), Admin, 0};
    admin.store(connection);

    Config::init(connection);
    if (!Config::exists("StudentBorrowLimit")) {
        Config::set("StudentBorrowLimit", "10");
    }
    if (!Config::exists("TeacherBorrowLimit")) {
        Config::set("TeacherBorrowLimit", "20");
    }
    if (!Config::exists("MaxBorrowDuration")) {
        Config::set("MaxBorrowDuration", "2592000000"); // millis of 30 days
    }
}

export auto sqlite = make_unique<SqliteConnection>("./data.db");