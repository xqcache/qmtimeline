```c++
// Timeline视图
class QmTLGraphicsView;
// Timeline场景
class QmTLGraphicsScene;
// Timeline模型
class QmTLGraphicsModel;

// Item数据，非Qt对象，仅负责存储数据
class QmTLItemData;
class QmTLFrameItemData;
class QmTLIOItemData;
class QmTLVideoItemData;
class QmTLAudioItemData;

// Item模型，负责处理Qt相关信号以及与Graphcis交互
class QmTLItemModel;
class QmTLFrameItemModel;
class QmTLIOItemModel;
class QmTLVideoItemModel;
class QmTLAudioItemModel;

// Item绘图，负责对应Item的绘制以及用户交互
class QmTLItemPrimitive;
class QmTLGraphicsFrame;
class QmTLGraphicsIO;
class QmTLGraphicsVideo;
class QmTLGraphicsAudio;

```



```json
{
    "[css]": {
        "editor.formatOnSave": false
    },
    "clangd.arguments": [
        "--header-insertion=never",
        "--clang-tidy",
        "--fallback-style=WebKit",
        // 指定clangd查找compile_commands.json的路径
        "--compile-commands-dir=${workspaceFolder}/build-dbg"
    ],
    "cmake.configureArgs": [
        // 生成compile_commands.json
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    ],
    "cmake.debugConfig": {
        "sourceFileMap": {
            // Qt源码路径映射。调试时可以进入Qt源码
            "c:\\Users\\qt\\work\\qt": "D:\\Qt\\6.8.1\\Src"
        },
        "visualizerFile": "${workspaceFolder}/.vscode/qt6.natvis",
        "displayString": true
    }
}
```