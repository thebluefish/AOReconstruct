#include "MainWindow.h"

#include <wx/wx.h>

class MainApplication : public wxApp
{
public:
	virtual bool OnInit();
	virtual void CleanUp();

private:

	AOReconstruct::MainWindow* _mainWindow;

};