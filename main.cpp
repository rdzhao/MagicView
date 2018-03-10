#include <QApplication>

#include "Viewer.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Viewer v;
	v.setMinimumSize(1200, 950);
	v.show();

	return a.exec();
}
