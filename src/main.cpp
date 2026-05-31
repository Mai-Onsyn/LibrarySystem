#include <iostream>
import Runtime;
import Application;

int main() {
    system("chcp 65001");
    sqlite->connect();
    initSql(sqlite.get());
    Application app;
    app.run();

    return 0;
}