module;
#include <memory>
#include "sqlite3.h"
export module SqliteConnection;
import Types;
import Format;

struct SqliteDeleter {
    void operator()(sqlite3* db) const { sqlite3_close_v2(db); }
};
struct StmtDeleter {
    void operator()(sqlite3_stmt* stmt) const { sqlite3_finalize(stmt); }
};

typedef std::unique_ptr<sqlite3, SqliteDeleter> SqlInstance;
typedef std::unique_ptr<sqlite3_stmt, StmtDeleter> PreparedStatement;

export class SqliteConnection {
    String dbFilePath;
    SqlInstance instance;

    void check(const Int result) const {
        if (result != SQLITE_OK &&
            result != SQLITE_DONE &&
            result != SQLITE_ROW) {
            throw RuntimeError(sqlite3_errmsg(instance.get()));
        }
    }

    template<typename T>
    void bindParam(const PreparedStatement& statement, const T& value, int& index) const {
        using RawType = std::remove_cvref_t<T>;
        const auto stmt = statement.get();
        if constexpr (std::is_same_v<RawType, Boolean>) {
            check(sqlite3_bind_int(stmt, index, value ? 1 : 0));
        }
        else if constexpr (std::is_integral_v<RawType>) {
            if constexpr (sizeof(RawType) <= 4) {
                check(sqlite3_bind_int(stmt, index, static_cast<Int>(value)));
            }
            else {
                check(sqlite3_bind_int64(stmt, index, static_cast<Long>(value)));
            }
        }
        else if constexpr (std::is_floating_point_v<RawType>) {
            check(sqlite3_bind_double(stmt, index, static_cast<Double>(value)));
        }
        else if constexpr (std::is_same_v<RawType, String>) {
            check(sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT));
        }
        else throw RuntimeError("Unsupported type");
        index++;
    }

    template<typename... Args>
    PreparedStatement prepare(const String& sql, Args&&... args) const {
        sqlite3_stmt* rawStatement = nullptr;
        check(sqlite3_prepare_v2(instance.get(), sql.c_str(), -1, &rawStatement, nullptr));

        PreparedStatement ps(rawStatement);
        Int index = 1;
        (bindParam(ps, std::forward<Args>(args), index), ...);
        return ps;
    }
public:
    explicit SqliteConnection(const String& filename) {
        this->dbFilePath = filename;
    }

    void connect() {
        sqlite3* rawDb = nullptr;
        if (const Int result = sqlite3_open(dbFilePath.c_str(), &rawDb);
            result != SQLITE_OK
        ) {
            const String error = rawDb ? sqlite3_errmsg(rawDb) : "Unknown sqlite error";
            if (rawDb) {
                sqlite3_close(rawDb);
            }
            throw RuntimeError(error);
        }
        instance.reset(rawDb);
    }

    void execute(const String& sql) const {
        char* errMsg = nullptr;
        if (const Int result = sqlite3_exec(instance.get(), sql.c_str(), nullptr, nullptr, &errMsg); result != SQLITE_OK) {
            const String error = errMsg ? errMsg : "Unknown sqlite error";
            sqlite3_free(errMsg);
            throw RuntimeError(error);
        }
    }

    template<typename... Args>
    void executeUpdate(const String& sql, Args&&... args) const {
        const PreparedStatement ps = prepare(sql, std::forward<Args>(args)...);
        check(sqlite3_step(ps.get()));
    }

    template<typename T, typename... Args>
    List<T> query(const String& sql, const Int column, Args&&... args) const {
        const PreparedStatement ps = prepare(sql, std::forward<Args>(args)...);

        using RawType = std::remove_cvref_t<T>;
        List<T> list;
        while (true) {
            const Int result = sqlite3_step(ps.get());
            if (result == SQLITE_ROW) {
                if constexpr (std::is_same_v<RawType, Boolean>) {
                    list.add(sqlite3_column_int(ps.get(), column) != 0);
                }
                else if constexpr (std::is_same_v<RawType, Int>) {
                    list.add(sqlite3_column_int(ps.get(), column));
                }
                else if constexpr (std::is_same_v<RawType, Long>) {
                    list.add(sqlite3_column_int64(ps.get(), column));
                }
                else if constexpr (std::is_floating_point_v<RawType>) {
                    list.add(sqlite3_column_double(ps.get(), column));
                }
                else if constexpr (std::is_same_v<RawType, String>) {
                    const auto text = reinterpret_cast<const char*>(
                        sqlite3_column_text(ps.get(), column)
                    );
                    list.add(text ? String(text) : String());
                }
                else throw RuntimeError("Unsupported type");
            }
            else if (result == SQLITE_DONE) {
                break;
            }
            else {
                check(result);
            }
        }
        return list;
    }

    Boolean containsID(const String& tableName, const Long id) const {
        auto result = query<Int>(Stringf::format("select 1 from %s where id = ? limit 1", tableName), 0, id);
        return !result.isEmpty();
    }
};