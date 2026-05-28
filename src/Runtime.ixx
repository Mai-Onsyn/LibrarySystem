module;
export module Runtime;
import SqliteConnection;
import Types;

export void initSql(const SqliteConnection* connection) {
    connection->execute(
        R"(create table if not exists Users (
                id integer primary key autoincrement,
                username text not null,
                password text not null,
                role integer not null,
                create_date integer not null
                ))"
        );
    connection->execute(
        R"(create table if not exists Books (
                id integer primary key autoincrement,
                title text not null,
                code text not null,
                type integer not null,
                status text not null,
                borrow_count integer not null
                ))"
        );
    connection->execute(
        R"(create table if not exists BorrowRecord (
                id integer primary key autoincrement,
                user_id integer not null,
                book_od integer not null,
                borrow_date integer not null,
                return_date integer
                ))"
        );
    connection->execute(
        R"(create table if not exists Config (
                id integer primary key autoincrement,
                key text not null,
                value text not null
                ))"
        );
}


export auto sqlite = make_unique<SqliteConnection>("./data.db");