# GTA San Andreas with AI
GTA San Andreas with AI（SAAI），将大语言模型(GPT)、TTS、SOVITS整合进入圣安地列斯，使用真正的AI控制NPC的行为、语音。

## 软件架构

开发环境：VS2022、Windows11

[前端(**Plugin**)](https://github.com/Katock-Cricket/SAAI.Plugin)：[SAAI.Plugin] ASI插件 

[后端(**Server**)](https://github.com/Katock-Cricket/SAAI.Server)：[SAAI.Server] EdgeTTS+SOVITS-SVC 

前后端通过socket实现IPC通信。

## [Plugin](https://github.com/Katock-Cricket/SAAI.Plugin)架构

功能：连接ChatGPT、组织AI行为、向AI语音Server发出内容生成的请求、操作游戏功能

### 配置信息

自写INIReader库读取配置信息，包含GPT的URL、API KEY、提示词、以及其他性能相关参数

### ChatGPT

使用Curl库调用ChatGPT API，支持国内中转，自写JSON库实现JSON构造和解析

### 逆向接口

基于[PluginSDK](https://github.com/DK22Pac/plugin-sdk)提供的逆向接口实现游戏功能操控，例如自定义作弊码、刷Special NPC、组织小弟、NPC说自定义音频、字幕等

### NPC说自定义音频

使用了音频ID映射算法，忘了从GTAForums的哪个帖子找到的了，具体是将每条音频都能映射到逆向接口能识别的ID。

### 中文字幕

桥接[无名汉化补丁](https://github.com/WMHHZ/VC.SA.Plugin)（直接调用PrintString[0x71A210]，传入UTF-8编码的中文，自动JMP到无名重写的函数），以实现实时内容的中文字幕

### 优化延时

AI语音和连接GPT都会花时间，所以AI行为都是异步的，编写了AIBeh的三级请求队列，用流水线的方式生成连续对话内容。

## [Server](https://github.com/Katock-Cricket/SAAI.Server)架构

功能：生成对应NPC音色的AI语音

### AI语音

EdgeTTS+[SOVITS-SVC](https://github.com/svc-develop-team/so-vits-svc)

轻度改写SOVITS以提供轻量快速的推理服务，整合了CPU版的Pytorch环境，无需手动配置。尽可能适应更多运行环境。

### 与Plugin通信

socket连接，Plugin提供人物+内容字段，Server生成对应的音频存储到计算的位置

### 实时音频替换

依托[Modloader](https://github.com/thelink2012/modloader)提供的reinstall功能，每隔1秒，新替换的音频会被自动挂载。



**持续迭代中…**

目前所有组件都是声明定义一体，后续东西多了可能会搞得好看点，把它们分开。

- [x] 支持中文对话
- [x] 若干轮对话后崩溃BUG
- [ ] 新BUG：8轮左右对话后变哑子
- [ ] 新BUG：GPT3.5持续产出新内容的能力较弱
- [ ] ~~改用RVC~~（延迟可接受(5秒)，暂不改动，主要影响在EdgeTTS上(网不好可能10秒)）
- [ ] 更多AI行为
- [ ] 仿5随机刷新的Special NPC

联系开发者：[B站_Cyber蝈蝈总](https://space.bilibili.com/37706580)
