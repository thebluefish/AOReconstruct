#include "main.h"

IMPLEMENT_APP(MainApplication)

bool MainApplication::OnInit()
{
	_mainWindow = new AOReconstruct::MainWindow();
	_mainWindow->Show(true);

	return true;
}

void MainApplication::CleanUp()
{

}