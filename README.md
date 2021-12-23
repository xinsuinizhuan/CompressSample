# 介绍
一个基于Qt实现的压缩软件示例，包含了一些软件下发的功能。

# 配置
支持如下自定义项：
1. 在`Common/ProductDef.h`中修改软件名称（中英文），可执行文件名称。
2. 在`Common/HttpManager.h`中修改API接口的地址。
3. 在`Common/HttpManager.cpp`中修改API接口的请求、解析方式。
4. 在`Common/Util.h`中修改API协议的解析方式。

# 编译
1. 安装Visual Studio 2019，需要安装英文语言包
2. 安装VCPKG
3. 打开compress.sln，编译