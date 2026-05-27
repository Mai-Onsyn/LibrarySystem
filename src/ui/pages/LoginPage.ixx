module;
#include <iostream>
export module pages.LoginPage;
import Format;
import Types;
import Logger;
import Page;
import PageState;

export class LoginPage final : public Page {
public:
    LoginPage() = default;

    PageState draw() override {
        cout << "Input User Name: ";
        String name = requestLine();
        cout << "User name is " << name << '\n';
        return PageState::Register;
    }
};