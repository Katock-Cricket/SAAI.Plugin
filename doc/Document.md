# 技术细节说明

## [Plugin](https://github.com/Katock-Cricket/SAAI.Plugin)架构

功能：连接ChatGPT、组织AI行为、向AI语音Server发出内容生成的请求、操作游戏功能

### 配置信息

自写INIReader库读取配置信息，包含GPT的URL、API KEY、提示词、以及其他性能相关参数

### ChatGPT

使用Curl库调用ChatGPT API，支持国内中转，自写JSON库实现JSON构造和解析

### 逆向接口

基于[PluginSDK](https://github.com/DK22Pac/plugin-sdk)提供的逆向接口实现游戏功能操控，例如自定义作弊码、刷Special NPC、组织小弟、NPC说自定义音频、字幕等

### NPC说自定义音频

使用了音频ID映射算法，从GTAForums的坟帖子找的，具体是将每条音频映射到接口能识别的唯一ID。

### 中文字幕

桥接[无名汉化补丁](https://github.com/WMHHZ/VC.SA.Plugin)（直接调用PrintString[0x71A210]，传入UTF-8编码的中文，自动JMP到无名重写的函数），以实现实时内容的中文字幕

### 优化延时

AI语音和连接GPT都会花时间，所以AI行为都是异步的，编写了AIBeh的若干级请求队列，用流水线的方式生成连续对话内容。以及更多细节优化。

## [Server](https://github.com/Katock-Cricket/SAAI.Server)架构

功能：生成对应NPC音色的AI语音

### AI语音

EdgeTTS+[SOVITS-SVC](https://github.com/svc-develop-team/so-vits-svc)

轻度改写SOVITS以提供轻量快速的推理服务，整合了CPU版的Pytorch环境，无需手动配置。尽可能适应更多运行环境。

### 与Plugin通信

socket连接，Plugin提供人物+内容字段，Server生成对应的音频存储到计算的位置

### 实时音频替换

依托[Modloader](https://github.com/thelink2012/modloader)提供的reinstall功能，每隔1秒，新替换的音频会被自动挂载。