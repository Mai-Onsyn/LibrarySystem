module;
#include <iostream>
export module pages.RegisterPage;
import Format;
import Types;
import Logger;
import Page;
import PageState;

export class RegisterPage final : public Page {
    PageState draw() override {
        cout << "Input User Name2: ";
        String name = requestInput<String>();
        cout << "User name2 is " << name << '\n';
        return PageState::Login;
    }
};