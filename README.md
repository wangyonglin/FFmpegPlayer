 #   FFmpegPlayer QT播放器 运行库
 ｀｀｀
 	#include <QResizeEvent>

	ChatWidget::ChatWidget(QWidget *parent)
	    : QWidget(parent)

	{
	    ffmpegplayer=new OpenGLFFmpegPlayer(this);
	    //ffmpegplayer=new FFmpegPlayer(this);

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

｀｀｀
    LIBS += -lavcodec
    LIBS += -lavdevice
    LIBS += -lavfilter
    LIBS += -lavformat
    LIBS += -lavutil
    LIBS += -lswscale
    LIBS += -lswresample
    LIBS += -lcblas
    LIBS += -Wall -lpthread
