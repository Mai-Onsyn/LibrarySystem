module;
#include <vector>
export module Config;
import Types;
import SqliteConnection;
import Hash;

export class Config {
    inline static const SqliteConnection* connection = nullptr;
public:
    static void init(const SqliteConnection* c) {
        connection = c;
    }

    static void set(const String& key, const String& value) {
        connection->executeUpdate(R"(
                INSERT INTO Config (id, key, value)
                VALUES (?, ?, ?)
                ON CONFLICT(id) DO UPDATE SET
                    value=excluded.value;
                )",
            hash64(key), key, value
        );
    }

    static String get(const String& key) {
        return connection->query<String>("SELECT value FROM Config WHERE key = ?", 0, key)[0];
    }

    static Boolean exists(const String& key) {
        return connection->containsID("Config", hash64(key));
    }
};