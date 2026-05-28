module;
#include <iostream>
export module pages.ExitPage;
import Format;
import Types;
import Logger;
import Page;
import PageState;
import User;

export class ExitPage final : public Page {
public:
    PageState draw() override {
        drawer.drawTip("真的要退出吗? (y/N)");
        if (boolInput()) {
            return PageState::Terminate;
        }
        switch (currentAccountType) {
            case Admin:
                return PageState::AdminMenu;
            case Teacher:
                return PageState::TeacherMenu;
            case Student:
                return PageState::StudentMenu;
            default:
                return PageState::Welcome;
        }
    }
};