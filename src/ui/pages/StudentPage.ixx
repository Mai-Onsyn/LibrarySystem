module;
#include <iostream>
export module pages.StudentPage;
import Format;
import Types;
import Logger;
import Page;
import PageState;

export class StudentPage final : public Page {
public:
    PageState draw() override {
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("Student Page");
        drawer.drawHorizontalLine();
        wait();
        return PageState::Welcome;
    }
};