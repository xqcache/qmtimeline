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
class QmTLGraphicsItem;
class QmTLGraphicsFrame;
class QmTLGraphicsIO;
class QmTLGraphicsVideo;
class QmTLGraphicsAudio;

```