module;
#include <memory>
#include "sqlite3.h"
export module SqliteConnection;
import Types;

struct SqliteDeleter {
    void operator()(sqlite3* db) const { sqlite3_close(db); }
};
struct StmtDeleter {
    void operator()(sqlite3_stmt* stmt) const { sqlite3_finalize(stmt); }
};

typedef unique_ptr<sqlite3, SqliteDeleter> SqlInstance;
typedef unique_ptr<sqlite3_stmt, StmtDeleter> PreparedStatement;

export class SqliteConnection {
    String dpFilePath;
    SqlInstance instance;

    template<typename T>
    void bindParam(const PreparedStatement& statement, const T& value, int& index) {
        const auto stmt = statement.get();
        if constexpr (std::is_integral_v<T>) {
            if constexpr (sizeof(T) <= 4) {
                sqlite3_bind_int(stmt, index, static_cast<Int>(value));
            }
            else {
                sqlite3_bind_int64(stmt, index, static_cast<Long>(value));
            }
        }
        else if constexpr (std::is_floating_point_v<T>) {
            sqlite3_bind_double(stmt, index, static_cast<Double>(value));
        }
        else if constexpr (std::is_same_v<T, String>) {
            sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
        }
        else throw RuntimeError("Unsupported type");
        index++;
    }
public:
    explicit SqliteConnection(const String& filename) {
        this->dpFilePath = filename;
    }

    Boolean connect() const {
        sqlite3* rawDb = nullptr;
        return sqlite3_open(dpFilePath.c_str(), &rawDb) == SQLITE_OK;
    }

    Boolean execute(const String& sql) const {
        return sqlite3_exec(instance.get(), sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK;
    }

    template<typename... Args>
    Boolean executeUpdate(const String& sql, Args... args) const {
        sqlite3_stmt* rawStatement = nullptr;

        if (sqlite3_prepare_v2(instance.get(), sql.c_str(), -1, &rawStatement, nullptr) == SQLITE_OK) {
            PreparedStatement ps(rawStatement);
            Int index = 1;
            (bindParam(ps, args, index), ...);
            return sqlite3_step(ps.get()) == SQLITE_DONE;
        }
        return false;
    }
};