module;
#include <iostream>
export module pages.AdminPage;
import Format;
import Types;
import Logger;
import Page;
import PageState;

export class AdminPage final : public Page {
public:
    PageState draw() override {
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("Admin Page");
        drawer.drawHorizontalLine();
        wait();
        return PageState::Welcome;
    }
};