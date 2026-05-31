module;
export module Storable;
import SqliteConnection;
import Types;

export class Storable {
public:
    virtual ~Storable() = default;

    virtual void store(const SqliteConnection* connection) = 0;

    static Storable* load(const SqliteConnection* connection);
};