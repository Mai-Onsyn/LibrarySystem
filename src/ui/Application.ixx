module;
#include <map>
#include <memory>
export module Application;
import Types;
import Format;
import PageState;
import Page;
import pages.WelcomePage;
import pages.LoginPage;
import pages.RegisterPage;
import pages.AdminPage;
import pages.TeacherPage;
import pages.StudentPage;
import pages.ExitPage;

export class Application {
    PageState currentPage = PageState::Welcome;
    Boolean isRunning = true;
    Map<PageState, UniquePtr<Page>> pages;

    void drawPage(const PageState page) {
        Page::clearScreen();
        switch (page) {
            case PageState::Welcome:
                setPage(pages[PageState::Welcome].get()->enter());
                break;
            case PageState::Login:
                setPage(pages[PageState::Login].get()->enter());
                break;
            case PageState::Register:
                setPage(pages[PageState::Register].get()->enter());
                break;
            case PageState::AdminMenu:
                setPage(pages[PageState::AdminMenu].get()->enter());
                break;
            case PageState::TeacherMenu:
                setPage(pages[PageState::TeacherMenu].get()->enter());
                break;
            case PageState::StudentMenu:
                setPage(pages[PageState::StudentMenu].get()->enter());
                break;
            case PageState::Exit:
                setPage(pages[PageState::Exit].get()->enter());
                break;
            case PageState::Terminate:
                isRunning = false;
                cout << "系统已正常退出\n";
                break;
            default:
                isRunning = false;
                break;
        }
    }
public:

    Application() {
        pages.emplace(PageState::Welcome, make_unique<WelcomePage>());
        pages.emplace(PageState::Login, make_unique<LoginPage>());
        pages.emplace(PageState::Register, make_unique<RegisterPage>());
        pages.emplace(PageState::AdminMenu, make_unique<AdminPage>());
        pages.emplace(PageState::TeacherMenu, make_unique<TeacherPage>());
        pages.emplace(PageState::StudentMenu, make_unique<StudentPage>());
        pages.emplace(PageState::Exit, make_unique<ExitPage>());
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

    [[nodiscard]] PageState getPage() const {
        return currentPage;
    }
};
