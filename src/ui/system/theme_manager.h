#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QList>
#include <QReadWriteLock>

// 前向声明，避免循环包含
class BaseWidget;

/**
 * @brief 全局主题管理器（单例）
 *
 * 负责：
 * 1. 管理应用当前主题状态（亮色/暗色）
 * 2. 维护所有已注册的BaseWidget组件列表
 * 3. 主题变更时，通知所有已注册组件更新
 * 4. 提供System模块专用的扩展颜色
 */
class ThemeManager : public QObject
{
    Q_OBJECT
    // 注意：UITheme枚举定义在base_widget.h中，这里使用int兼容属性
    Q_PROPERTY(int currentTheme READ currentThemeInt WRITE setCurrentThemeInt NOTIFY themeChanged)

public:
    // 删除拷贝构造函数和赋值运算符，确保单例
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    /**
     * @brief 获取单例实例
     * @return ThemeManager单例指针
     */
    static ThemeManager* instance();
    static ThemeManager* safeInstance();

    // ==================== 核心功能：主题状态管理 ====================

    /**
     * @brief 获取当前主题
     * @return 当前主题枚举值
     */
    int currentThemeInt() const;

    /**
     * @brief 设置当前主题
     * @param theme 主题枚举值（0=亮色, 1=暗色）
     *
     * 设置新主题后，会自动：
     * 1. 重载系统扩展颜色
     * 2. 通知所有已注册的BaseWidget组件更新
     * 3. 发射themeChanged信号
     */
    void setCurrentThemeInt(int theme);

    /**
     * @brief 切换亮色/暗色主题
     */
    void toggleTheme();

    // ==================== 核心功能：组件注册 ====================

    /**
     * @brief 注册BaseWidget组件
     * @param widget 要注册的组件指针
     *
     * BaseWidget应在构造时调用此方法进行注册
     */
    void registerWidget(BaseWidget* widget);

    /**
     * @brief 反注册BaseWidget组件
     * @param widget 要反注册的组件指针
     *
     * BaseWidget应在析构时调用此方法进行反注册
     */
    void unregisterWidget(BaseWidget* widget);

    // ==================== 颜色获取 ====================

    /**
     * @brief 获取System模块扩展的系统专用颜色
     * @param role 颜色角色名称
     * @return 颜色值（十六进制字符串），如果角色不存在则返回透明色
     *
     * 这些颜色是BaseWidget颜色表中没有的，专门用于System模块
     * 例如："sidebar.background", "sidebar.border"等
     */
    QString getSystemColor(const QString& role) const;

    /**
     * @brief 获取所有可用的系统颜色角色
     * @return 颜色角色列表
     */
    QStringList availableSystemColorRoles() const;

signals:
    /**
     * @brief 主题已变更信号
     * @param newTheme 新主题枚举值
     *
     * 当主题变更并已通知所有注册组件后发射
     */
    void themeChanged(int newTheme);

private:
    // 私有构造函数，强制单例
    explicit ThemeManager(QObject* parent = nullptr);

    // 新增：单例销毁标志（静态成员变量）
    static std::atomic<bool> isDestroyed;

    // 析构函数
    ~ThemeManager();

    /**
     * @brief 加载/重载系统扩展颜色表
     *
     * 根据当前主题加载对应的系统颜色配置
     */
    void loadSystemColors();

    // 单例实例指针
    //static ThemeManager* m_instance;


    // 当前主题状态 (0=Light, 1=Dark)
    int m_currentTheme{0};

    // 所有已注册的BaseWidget组件列表
    QList<BaseWidget*> m_registeredWidgets;

    // 读写锁，保护组件列表的线程安全
    QReadWriteLock m_widgetListLock;

    // 系统扩展颜色表 (例如: "sidebar.background" -> "#f8fafc")
    QMap<QString, QString> m_systemColors;
};

#endif // THEME_MANAGER_H
