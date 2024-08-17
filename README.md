 #   FFmpegPlayer 视频播放器模块 
## 默认使用依赖库版本信息

	1.FFmpeg4.4.1
	2.Qt 5.12.8

### 默认生成动态库文件

```shell
wangyonglin@wangyonglin-macmini:~/github/build-FFmpegPlayer-Rk3588_Buildroot_aarch64-Debug$ ls libFFmpegPlayer*
libFFmpegPlayer.so    libFFmpegPlayer.so.1.0
libFFmpegPlayer.so.1  libFFmpegPlayer.so.1.0.0
```

 ### OpenGLFFmpegPlayer 采用 QOpenGLWidget opengl 渲染 Qimage

```c++
	// 使用例子
	#include <QResizeEvent>
	ChatWidget::ChatWidget(QWidget *parent)
	    : QWidget(parent)

	{
	    ffmpegplayer=new OpenGLFFmpegPlayer(this);
	    ffmpegplayer->resizeGL(800,600);
	    ffmpegplayer->Play("https://cesium.com/public/SandcastleSampleData/big-buck-bunny_trailer.mp4");
	}

	ChatWidget::~ChatWidget()
	{
	    ffmpegplayer->Stop();

	}

	void ChatWidget::resizeEvent(QResizeEvent *event)
	{
	     ffmpegplayer->resize(event->size());
	}
```

### FFmpegPlayer 采用 QWidget paintEvent QPainter 渲染 Qimage

```c++
	// 使用例子
	#include <QResizeEvent>
	ChatWidget::ChatWidget(QWidget *parent)
	    : QWidget(parent)

	{

	    ffmpegplayer=new FFmpegPlayer(this);
	    ffmpegplayer->resize(800,600);
	    ffmpegplayer->Play("https://cesium.com/public/SandcastleSampleData/big-buck-bunny_trailer.mp4");
	}

	ChatWidget::~ChatWidget()
	{
	    ffmpegplayer->Stop();

	}

	void ChatWidget::resizeEvent(QResizeEvent *event)
	{
	     ffmpegplayer->resize(event->size());
	}
```
### 主项目的 *.pro
#### 增加需要的信赖库参考配置 ffmpeg  
```pro
   QT       += core gui multimedia opengl

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets openglwidgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



CONFIG += debug_and_release
CONFIG(debug, debug|release){
    unix{
        contains(QT_ARCH, arm64){
        message("arm64")
        INCLUDEPATH += /home/wangyonglin/github/FFmpegPlayer
        LIBS += -L$$PWD/lib/unix/aarch64 -lFFmpegPlayer

        INCLUDEPATH += $$PWD/include/ffmpeg/include
        LIBS += -L$$PWD/lib/unix/aarch64 -lavcodec
        LIBS += -L$$PWD/lib/unix/aarch64 -lavdevice
        LIBS += -L$$PWD/lib/unix/aarch64 -lavfilter
        LIBS += -L$$PWD/lib/unix/aarch64 -lavformat
        LIBS += -L$$PWD/lib/unix/aarch64 -lavutil
        LIBS += -L$$PWD/lib/unix/aarch64 -lswscale
        LIBS += -L$$PWD/lib/unix/aarch64 -lswresample
        #LIBS += -L/home/wangyonglin/Qt/sysroot/usr/lib/aarch64-linux-gnu -lcblas
        }else{
        message("x86")
        INCLUDEPATH += /home/wangyonglin/github/FFmpegPlayer
        LIBS +=  -L$$PWD/lib/unix/x86_64 -lFFmpegPlayer

        LIBS +=  -L$$PWD/lib/unix/x86_64 -lavcodec
        LIBS +=  -L$$PWD/lib/unix/x86_64 -lavdevice
        LIBS +=  -L$$PWD/lib/unix/x86_64 -lavfilter
        LIBS +=  -L$$PWD/lib/unix/x86_64 -lavformat
        LIBS +=  -L$$PWD/lib/unix/x86_64 -lavutil
        LIBS +=  -L$$PWD/lib/unix/x86_64 -lswscale
        LIBS +=  -L$$PWD/lib/unix/x86_64 -lswresample
        LIBS +=  -L$$PWD/lib/unix/x86_64 -lcblas

        }
        LIBS += -Wall -lpthread
    }
}

SOURCES += \
    main.cpp \
    ChatWidget.cpp

HEADERS += \
    ChatWidget.h

TRANSLATIONS += \
    ChatPlayer_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md
   
```