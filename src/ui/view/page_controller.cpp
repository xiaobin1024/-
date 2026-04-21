#include "page_controller.h"
#include <QVBoxLayout>
#include <QDebug>

PageController::PageController(QWidget* parent)
    : BaseWidget(parent)
{
    qDebug() << "PageController 创建";
   initialize();
    setupPages();
    connectPageSignals();

    // 初始显示登录页面
    setCurrentPage(Login);
}

PageController::~PageController()
{
    qDebug() << "PageController 销毁";
}

void PageController::initUI()
{
    qDebug()<<"PageController::initUI()";
    BaseWidget::initUI();
    setObjectName("PageController");
    setWindowTitle("网络词典 - 登录/注册");
}

void PageController::setupLayout()
{
    qDebug()<<"PageController::setupLayout()";
    m_stackedWidget = new QStackedWidget(this);
    m_mainLayout->addWidget(m_stackedWidget);
}

void PageController::setupPages()
{
    // 创建页面
    m_loginPage = new LoginPage(this);
    m_registerPage = new RegisterPage(this);
    m_mainPage = new MainPage(this);
    m_collectPage = new CollectePage(this);
    m_vocabularyPage = new VocabularyPage(this);

    // 添加到堆栈控件
    m_stackedWidget->addWidget(m_loginPage);
    m_stackedWidget->addWidget(m_registerPage);
    m_stackedWidget->addWidget(m_mainPage);
    m_stackedWidget->addWidget(m_collectPage);
    m_stackedWidget->addWidget(m_vocabularyPage);

    ExportManager::instance()->bindToCollectePage(m_collectPage);
    ExportManager::instance()->bindToVocabularyPage(m_vocabularyPage);
}

void PageController::connectPageSignals()
{
    qDebug()<<"PageController::connectPageSignals()";

    // 安全检查
    if (!m_loginPage || !m_registerPage || !m_mainPage) {
        qWarning() << "PageController::connectPageSignals() - 页面对象未初始化";
        return;
    }
    // 连接登录页面信号
    connect(m_loginPage, &LoginPage::loginSuccess,
            this, &PageController::handleLoginSuccess);
    connect(m_loginPage, &LoginPage::navigateToRegister,
            this, &PageController::handleNavigateToRegister);
    connect(m_loginPage, &LoginPage::navigateToMain,
            this, &PageController::handleNavigateToMain);

    // 连接注册页面信号
    connect(m_registerPage, &RegisterPage::registerSuccess,
            this, &PageController::handleRegisterSuccess);
    connect(m_registerPage, &RegisterPage::navigateToLogin,
            this, &PageController::handleNavigateToLogin);
    connect(m_registerPage, &RegisterPage::navigateToMain,
            this, &PageController::handleNavigateToMain);

    // 连接主页面信号
    connect(m_mainPage, &MainPage::navigateToLogin,
            this, &PageController::handleNavigateToLogin);
    connect(m_mainPage, &MainPage::navigateToRegister,
            this, &PageController::handleNavigateToRegister);

    connect(m_mainPage, &MainPage::showCollectPageRequested,
            this, &PageController::handleNavigateToCollect);

    connect(m_collectPage,&CollectePage::showMainPageRequested,
            this,&PageController::handleCollectToMain);

    connect(m_mainPage,&MainPage::showVocabularyRequested,this,&PageController::handleNavigateToVocabulary);
    connect(m_vocabularyPage,&VocabularyPage::showMainPageRequested,this,&PageController::handleVocabularyToMain);
}

void PageController::setCurrentPage(PageType pageType)
{
    m_stackedWidget->setCurrentIndex(pageType);

    // 可选：显示页面
    switch(pageType) {
    case Login:
        m_loginPage->showPage();
        break;
    case Register:
        m_registerPage->showPage();
        break;
    case Main:
        if (m_mainPage) m_mainPage->showPage();
        break;
    case Collect:
        m_collectPage->showPage();
        break;
    case Vocabulary:
        m_vocabularyPage->showPage();
        break;
    }

}

void PageController::handleLoginSuccess(const UserData& user)
{
    emit loginSuccess(user);
    // 可以选择跳转到主页面
    setCurrentPage(Main);
}

void PageController::handleRegisterSuccess(const UserData& user)
{
    emit registerSuccess(user);
    // 注册成功后跳转到登录页面
    setCurrentPage(Login);
}

void PageController::handleNavigateToRegister()
{
    setCurrentPage(Register);
}

void PageController::handleNavigateToLogin()
{
    setCurrentPage(Login);
}

void PageController::handleNavigateToMain()
{
    // 发射信号，让主窗口处理跳转到主页面
    UserData currentUser = UserSession::instance()->currentUser();
    if (currentUser.isLoggedIn()) {
        emit loginSuccess(currentUser);
    }
}

void PageController::handleNavigateToCollect()
{
    qDebug()<<"PageController::handleNavigateToCollect()";
    qDebug()<<"准备跳转收藏页面";
    setCurrentPage(Collect);
}

void PageController::handleCollectToMain()
{
    qDebug()<<" PageController::handleCollectToMain()";
    setCurrentPage(Main);
}

void PageController::handleNavigateToVocabulary()
{
    qDebug()<<"PageController::handleNavigateToVocabulary()";
    qDebug()<<"准备跳转生词本页面";
    setCurrentPage(Vocabulary);
}

void PageController::handleVocabularyToMain()
{
    qDebug()<<" PageController::handleVocabularyToMain()";
    setCurrentPage(Main);
}
