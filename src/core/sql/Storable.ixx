module;
export module Storable;
import SqliteConnection;
import Types;

export class Storable {
public:
    virtual ~Storable() = default;

    virtual void store(SqliteConnection* connection) = 0;

    virtual Storable* load(SqliteConnection* connection) = 0;
};