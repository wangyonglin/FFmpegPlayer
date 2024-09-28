#include <QCoreApplication>
#include <QApplication>
#include <Qt6FFmpeg/FFmpegPlayer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qt6FFmpeg::FFmpegPlayer player;
    player.resize(800,600);
   // player.play("https://media.w3.org/2010/05/sintel/trailer.mp4");
    //player.play("http://vjs.zencdn.net/v/oceans.mp4");
    player.play("https://cesium.com/public/SandcastleSampleData/big-buck-bunny_trailer.mp4");
    a.connect(&player,&Qt6FFmpeg::FFmpegPlayer::finished,[&](){
    player.stop();
    });
    player.show();
    return a.exec();
}

