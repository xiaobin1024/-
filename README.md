# 网络电子词典 - 类关系设计说明

## 架构概览

本项目采用**分层架构**设计，分为四个主要层次：

### 1. 界面层 (UI Layer)

* **MainWindow**: 应用程序主窗口，负责协调各个组件
* **PageManager**: 页面管理器，实现页面导航和生命周期管理
* **BasePage**: 所有页面的基类，提供统一的页面模板
* **具体页面类**: MainMenuPage, LoginPage, QueryPage 等

### 2. 业务逻辑层 (Business Logic Layer)

* **IMessageHandler**: 消息处理器接口，定义处理规范
* **具体处理器**: LoginHandler, RegisterHandler, QueryHandler
* **MessageHandlerFactory**: 工厂类，负责创建消息处理器实例

### 3. 网络层 (Network Layer)

* **INetworkManager**: 网络管理接口
* **NetworkManager**: 网络管理实现，处理TCP通信
* **Msg/MsgType**: 消息数据结构定义

### 4. 配置层 (Configuration Layer)

* **ConfigFactory**: 配置工厂，管理应用级配置
* **PageConfigManager**: 页面配置管理器
* **PageConfig**: 页面配置数据结构

## 核心设计模式应用

### 工厂模式 (Factory Pattern)
