#include <iostream>
import Format;
import Types;
import Application;
import Hash;
import Time;
import Runtime;

int main() {

    sqlite->connect();
    initSql(sqlite.get());
    system("chcp 65001");
    Application app;
    app.run();

    return 0;
}