
#include "qttesting.h"
#include "QtWidgets\qapplication.h"


int mainGUIMAUN(int argc, char* argv[])
{

	QApplication a(argc, argv);
	QtTesting w;
	w.show();
	a.exec();

	system("pause");
	return 0;
}