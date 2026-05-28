module;
#include <iostream>
export module pages.TeacherPage;
import Format;
import Types;
import Logger;
import Page;
import PageState;

export class TeacherPage final : public Page {
public:
    PageState draw() override {
        drawer.drawHorizontalLine();
        drawer.drawBorderedLine("Teacher Page");
        drawer.drawHorizontalLine();
        wait();
        return PageState::Welcome;
    }
};