#include <iostream>
import Format;

int main() {
    std::cout << format("This %s a test %%string%% int %d, boolean %b and float %.2f, is it ok?\n", "is", 42, false, 3.1415926f);
    std::cout << format("bc%%aa");
    return 0;
}