QT += gui core gui multimedia opengl widgets


TEMPLATE = lib

DEFINES += FFMPEGPLAYER_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AudioDecoder.cpp \
    FFmpegDemuxer.cpp \
    FFmpegFrame.cpp \
    FFmpegManager.cpp \
    FFmpegPacket.cpp \
    FFmpegPlayer.cpp \
    FFmpegResampler.cpp \
    FFmpegSpeaker.cpp \
    FFmpegSynchronizer.cpp \
    FFmpegThreader.cpp \
    OpenGLFFmpegPlayer.cpp \
    VideoDecoder.cpp

HEADERS += \
    AudioDecoder.h \
    FFmpegDemuxer.h \
    FFmpegFrame.h \
    FFmpegManager.h \
    FFmpegPacket.h \
    FFmpegPlayer_global.h \
    FFmpegPlayer.h \
    FFmpegResampler.h \
    FFmpegSpeaker.h \
    FFmpegSynchronizer.h \
    FFmpegThreader.h \
    OpenGLFFmpegPlayer.h \
    VideoDecoder.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    FFmpegPlayer.pro.user \
    OpenGLFFmpegPlayer.fsh \
    OpenGLFFmpegPlayer.vsh \
    READMD.md

RESOURCES += \
    resources.qrc
