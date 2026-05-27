module;
#include <map>
#include <memory>
export module Application;
import Types;
import Format;
import PageState;
import Page;
import pages.LoginPage;
import pages.RegisterPage;

export class Application {
    PageState currentPage = PageState::Login;
    Boolean isRunning = true;
    Map<PageState, UniquePtr<Page>> pages;

    void drawPage(const PageState page) {
#if _WIN32
        system("cls");
#else
        system("clear");
#endif
        switch (page) {
            case PageState::Login:
                setPage(pages[PageState::Login].get()->enter());
                break;
            case PageState::Register:
                setPage(pages[PageState::Register].get()->enter());
                break;
            case PageState::AdminMenu:
                break;
            case PageState::TeacherMenu:
                break;
            case PageState::StudentMenu:
                break;
            case PageState::Exit:
                break;
            default:
                isRunning = false;
                break;
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
public:

    Application() {
        // pages.add(make_unique<LoginPage>());
        // pages.add(make_unique<RegisterPage>());
        pages.emplace(PageState::Login, make_unique<LoginPage>());
        pages.emplace(PageState::Register, make_unique<RegisterPage>());
    }

    void run() {
        isRunning = true;
        while (isRunning) {
            drawPage(currentPage);
        }
    }

    void setPage(const PageState page) {
        this->currentPage = page;
    }

    PageState getPage() const {
        return currentPage;
    }
};
