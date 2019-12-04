#include "Render.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	Render w;
    
	w.setWindowState(Qt::WindowFullScreen);
	w.show();

	return a.exec();
}
