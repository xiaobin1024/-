#ifndef CORE_H
#define CORE_H

namespace CoreModule {
// 只保留最简工具函数
int calculateSum(int a, int b);

class CoreHandler {
public:
    void setReady(bool ready); // 最简方法：直接设置状态
    bool isReady() const;      // 获取状态
private:
    bool is_ready_ = false;
};
}

#endif // CORE_H
