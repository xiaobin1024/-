#include "theme_manager.h"
#include "base/base_widget.h"
#include <QDebug>
#include <QReadWriteLock>
#include <QWriteLocker>
#include <QReadLocker>
#include<QMutex>

// 初始化静态成员变量
//ThemeManager* ThemeManager::m_instance = nullptr;
std::atomic<bool> ThemeManager::isDestroyed(false);

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent)
{
    // 初始加载系统颜色
    loadSystemColors();
    qDebug() << "ThemeManager 初始化完成，当前主题：" << (m_currentTheme == 0 ? "亮色" : "暗色");
}

ThemeManager::~ThemeManager()
{
    // 修复4: 安全清理注册组件
    {
        QWriteLocker locker(&m_widgetListLock);
        m_registeredWidgets.clear();
    }

    qDebug() << "ThemeManager 析构完成";
    isDestroyed = true; // 标记为已销毁
}

ThemeManager* ThemeManager::instance()
{
    return safeInstance();
}

ThemeManager* ThemeManager::safeInstance()
{
    static ThemeManager instance;
    if (isDestroyed) {
        return nullptr;
    }

    return &instance;
}

int ThemeManager::currentThemeInt() const
{
    return m_currentTheme;
}

void ThemeManager::setCurrentThemeInt(int theme)
{
    // 验证主题值有效性
    if (theme != 0 && theme != 1) {
        qWarning() << "无效的主题值：" << theme << "，使用默认亮色主题";
        theme = 0;
    }

    if (m_currentTheme == theme) {
        return;
    }

    int oldTheme = m_currentTheme;
    m_currentTheme = theme;

    qDebug() << "主题变更：" << (oldTheme == 0 ? "亮色" : "暗色")
             << " -> " << (m_currentTheme == 0 ? "亮色" : "暗色");

    // 1. 重载系统扩展颜色
    loadSystemColors();

    // 2. 通知所有已注册的组件更新主题
    {
        QReadLocker locker(&m_widgetListLock);
        for (BaseWidget* widget : m_registeredWidgets) {
            if (widget) {
                // 将int转换为BaseWidget::UITheme枚举
                UITheme uiTheme = (theme == 0)
                                                  ? UITheme::Light
                                                  : UITheme::Dark;

                // 调用BaseWidget的公共方法更新主题
                widget->setUITheme(uiTheme);
            }
        }
    }

    // 3. 发射全局主题变更信号
    emit themeChanged(m_currentTheme);

    qDebug() << "已通知" << m_registeredWidgets.size() << "个组件主题更新";
}

void ThemeManager::toggleTheme()
{
    // 切换主题：0<->1
    int newTheme = (m_currentTheme == 0) ? 1 : 0;
    setCurrentThemeInt(newTheme);
}

void ThemeManager::registerWidget(BaseWidget* widget)
{
    if (!widget) {
        qWarning() << "尝试注册空的BaseWidget指针";
        return;
    }

    QWriteLocker locker(&m_widgetListLock);
    if (!m_registeredWidgets.contains(widget)) {
        m_registeredWidgets.append(widget);
        qDebug() << "注册BaseWidget：" << widget->metaObject()->className()
                 << "，当前注册数：" << m_registeredWidgets.size();
    }
}

void ThemeManager::unregisterWidget(BaseWidget* widget)
{
    if (!widget) {
        qWarning() << "尝试反注册空的BaseWidget指针";
        return;
    }

    QWriteLocker locker(&m_widgetListLock);
    int removed = m_registeredWidgets.removeAll(widget);
    if (removed > 0) {
        qDebug() << "反注册BaseWidget：" << widget->metaObject()->className()
            << "，当前注册数：" << m_registeredWidgets.size();
    }
}

QString ThemeManager::getSystemColor(const QString& role) const
{
    // 如果角色存在，返回对应颜色
    if (m_systemColors.contains(role)) {
        return m_systemColors.value(role);
    }

    // 角色不存在，返回透明色并警告
    qWarning() << "未找到系统颜色角色：" << role;
    return "#00000000"; // 透明色
}

QStringList ThemeManager::availableSystemColorRoles() const
{
    return m_systemColors.keys();
}

void ThemeManager::loadSystemColors()
{
    m_systemColors.clear();

    if (m_currentTheme == 1) { // 暗色主题
        m_systemColors = {
            // 侧边栏专用颜色
            {"sidebar.background", "#2d3748"},  // 比原来稍亮一些
            {"sidebar.border", "#4a5568"},       // 比原来更明显
            {"sidebar.button.background", "transparent"},
            {"sidebar.button.hover", "#3b82f620"},  // 20%透明度
            {"sidebar.button.pressed", "#3b82f640"}, // 40%透明度
            {"sidebar.button.icon", "#94a3b8"},
            {"sidebar.button.text", "#cbd5e1"},
            {"sidebar.button.text.disabled", "#64748b"},

            // 动画遮罩
            {"sidebar.mask", "#00000040"},

            // 扩展用颜色
            {"danger", "#ef4444"},
            {"danger.hover", "#dc262620"},
            {"danger.pressed", "#dc262640"},

            {"warning", "#f59e0b"},
            {"warning.hover", "#d9770620"},
            {"warning.pressed", "#d9770640"}
        };
    } else { // 亮色主题
        m_systemColors = {
            {"sidebar.background", "#f1f5f9"},  // 比原来稍暗一些
            {"sidebar.border", "#cbd5e1"},       // 比原来更深
            {"sidebar.button.background", "transparent"},
            {"sidebar.button.hover", "#3b82f610"},
            {"sidebar.button.pressed", "#3b82f620"},
            {"sidebar.button.icon", "#64748b"},
            {"sidebar.button.text", "#475569"},
            {"sidebar.button.text.disabled", "#94a3b8"},

            {"sidebar.mask", "#00000010"},

            {"danger", "#dc2626"},
            {"danger.hover", "#b91c1c20"},
            {"danger.pressed", "#b91c1c40"},

            {"warning", "#d97706"},
            {"warning.hover", "#b4530920"},
            {"warning.pressed", "#b4530940"}
        };
    }

    qDebug() << "已加载" << m_systemColors.size() << "个系统扩展颜色";
}
