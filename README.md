# CC Game Framework Project

参见：[从零开始构建标准库 - 知乎专栏](https://zhuanlan.zhihu.com/learncpp)

实现一个游戏框架。

**已嵌入物理引擎代码[clib2d](https://github.com/bajdcc/clib2d)。**

**已嵌入脚本操作系统代码[clibparser](https://github.com/bajdcc/clibparser)。**

## 脚本操作系统

简介：语言为简化C语言，自制编译器（词法+语法+LR分析）、IR（基于寄存器）、虚拟机、GUI、VFS

原理：执行程序时，即时编译代码并执行所生成的二进制指令，按需加载。

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/os_1.png)

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/os_2.png)


## 介绍

架构是**C/C++**，渲染用**DirectX**，库用**STL/ATL**。

从易到难，逐步推进。

使用的开源项目：

- libqrencode-3.4.4
- libevent-2.0.22
- libcurl-7.53.1
- dkjson(lua)

## 知乎文章列表

- [开篇](https://zhuanlan.zhihu.com/p/25027543)
- [初见成效](https://zhuanlan.zhihu.com/p/25184604)
- [有模有样](https://zhuanlan.zhihu.com/p/25284597)
- [2048](https://zhuanlan.zhihu.com/p/25401920)
- [诗情画意](https://zhuanlan.zhihu.com/p/25476629)
- [柳岸花明](https://zhuanlan.zhihu.com/p/25593280)
- [Wireworld元胞自动机](https://zhuanlan.zhihu.com/p/25593938)
- [余音袅袅](https://zhuanlan.zhihu.com/p/25717640)
- [简单的图形学（一）](https://zhuanlan.zhihu.com/p/30864679)
- [简单的图形学（二）——材质与反射](https://zhuanlan.zhihu.com/p/31012319)
- [简单的图形学（三）——光源](https://zhuanlan.zhihu.com/p/31015884)
- [五彩缤纷](https://zhuanlan.zhihu.com/p/31017634)
- [用C++画光（一）——优化](https://zhuanlan.zhihu.com/p/32150887)
- [用C++画光（二）——矩形](https://zhuanlan.zhihu.com/p/32251040)
- [用C++画光（三）——色散](https://zhuanlan.zhihu.com/p/32486185)
- [嵌入Bochs](https://zhuanlan.zhihu.com/p/33517169)

## 历程

### 第一阶段：搭好脚手架

项目架构：

- base - 基础类（包含二维码库、libevent、libcurl）
- ui - 界面逻辑
- render - DX渲染逻辑
- lua - lua代码
- lua_ext - lua扩展（UI、Web）
- script - lua脚本（UI、Scene）

整体思路：

- 将窗口包装成Window类，借鉴自MFC中的相关内容
- D2D1和DWrite的初始化
- 实现图元工厂模式，统一管理DX资源，已实现色块和文字的渲染
- 截取Window的特定消息，进行渲染

当前进度：

- 已整合Lua 5.3.3
- 实现背景渐变的效果
- 实现定时器功能
- 监听窗口消息
- 实现场景切换功能
- 实现绝对布局，可以自动调整大小
- 整合WIC图片工厂模块
- 整合二维码模块(libqrencode)
- 监听鼠标事件
- 添加线性布局
- 添加按钮控件
- 添加文本框控件
- 添加表格布局
- 添加HTTP组件，整合libevent和libcurl
- 添加图片控件，接收base64编码的图片二进制指针
- 添加lua版本的json解析
- 完善HTTP组件(GET,POST)
- 添加音频播放组件(libzplay)

TODO：

- 完善一言界面
- 添加更多控件
- 添加更多渲染图元

### 第二阶段：移植应用

- 游戏
  - 贪吃蛇
  - **2048（已完成）**
  - 俄罗斯方块
- 工具
  - 计算器（计划中）
  - JSON校验器
- 网络
  - **一言（已完成）**
  - **网易云音乐播放器（制作中）**
- 算法可视化
  - A*寻路（已完成）
  - **Wireworld 元胞自动机（已完成）**
  - **光线追踪（进行中）**
  
### 第三阶段：图形学

#### 光线追踪

- [用JavaScript玩转计算机图形学(一)光线追踪入门 - Milo Yip](http://www.cnblogs.com/miloyip/archive/2010/03/29/1698953.html) **（已完成）**
- [用JavaScript玩转计算机图形学(二)基本光源 - Milo Yip](http://www.cnblogs.com/miloyip/archive/2010/04/02/1702768.html) **（已完成）**
- [用 C 语言画光（一）：基础](https://zhuanlan.zhihu.com/p/30745861) **（已完成）**
- [用 C 语言画光（二）：构造实体几何](https://zhuanlan.zhihu.com/p/30748318) **（已完成）**
- [用 C 语言画光（三）：形状](https://zhuanlan.zhihu.com/p/30816284) **（已完成）**
- [用 C 语言画光（四）：反射](https://zhuanlan.zhihu.com/p/30961545) **（已完成）**

#### 2D物理引擎

物理引擎系列：

- [开篇](http://zhuanlan.zhihu.com/p/42669063)
- [创建窗口](http://zhuanlan.zhihu.com/p/42773209)


#### 截图

**主界面**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/gui_main.png)

**画光系列！！！（算法优化）**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/colors_1.png)

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/colors_2.png)

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/colors_3.png)

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/colors_4.png)

**光线追踪入门**

**1. 渐变**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/gui_1.png)

**2. 深度**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/gui_2.png)

**3. 材质**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/gui_3.png)

**4. 反射**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/gui_4.png)

**基本光源**

**1. 平行光**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/gui_5.png)

**1. 平行光（带阴影）**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/gui_5s.png)

**2. 点光源（带阴影）**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/gui_6.png)

**3. 聚光灯（带阴影）**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/gui_7.png)

**4. 三原色（带阴影）**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/gui_8.png)

**画光系列**

（采用动态刷新法）

**1. 抖动采样**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/light_1.png)

**2. 实体几何**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/light_2.png)

**3. 反射**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/light_3.png)

**画光系列（彩色）**

**1. 实体几何**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/color_1.png)

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/color_1a.png)

**2. 反射**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/color_2.png)

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/color_2a.png)

**3. 三原色**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/color_3.png)

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/color_3a.png)

**4. 折射**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/color_4.png)

**5. 折射（菲涅耳方程）**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/color_5.png)

**6. 折射（比尔-朗伯定律）**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/color_6.png)

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/color_6a.png)

**画光系列（文字）**

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/font_1.png)

![main](https://raw.githubusercontent.com/bajdcc/GameFramework/master/screenshots/font_2.png)

### 第四阶段：模拟器

#### Bochs

![](https://pic1.zhimg.com/v2-ef278e000706d563aabe9181d18481f6_r.jpg)

#### 脚本操作系统

- [【Parser系列】实现LR分析——开篇](https://zhuanlan.zhihu.com/p/52478414)
- [【Parser系列】实现LR分析——生成AST](https://zhuanlan.zhihu.com/p/52528516)
- [【Parser系列】实现LR分析——支持C语言文法](https://zhuanlan.zhihu.com/p/52812144)
- [【Parser系列】实现LR分析——完成编译器前端！](https://zhuanlan.zhihu.com/p/53070412)
- [【Parser系列】实现LR分析——识别变量声明](https://zhuanlan.zhihu.com/p/54716082)
- [【Parser系列】实现LR分析——解决二义性问题](https://zhuanlan.zhihu.com/p/54766453)
- [【虚拟机系列】C语言虚拟机诞生！](https://zhuanlan.zhihu.com/p/55319251)
- [【虚拟机系列】实现Fork！](https://zhuanlan.zhihu.com/p/55613027)

进度：

- [x] **一切皆文件**（抽象包括：文件、管道、队列、http客户端、http服务端、日志、进程、句柄、设备、音乐等）
- [x] Shell
- [x] 红黑树
- [x] GUI接口
- [x] 多窗口管理
- [x] IPC
- [x] HTTP
- [x] JSON
- [ ] 文件权限
- [ ] 多用户