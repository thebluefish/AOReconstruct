#include "MainWindow.h"

#include "AOHandler.h"

namespace AOReconstruct
{
	wxDEFINE_EVENT(wxEVT_STATUS_UPDATE, wxThreadEvent);

	wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
		EVT_THREAD(wxEVT_STATUS_UPDATE, MainWindow::OnAOScanUpdate)
	wxEND_EVENT_TABLE()

	MainWindow::MainWindow()
		: wxFrame(NULL, wxID_ANY, "AXS-One Reconstructor", wxPoint(0, 0), wxSize(600, 300)),
		_scanThread(0)
	{
			
		CreateLayout();
		Initialize();
	}

	MainWindow::~MainWindow()
	{
		if (_scanThread)
		{
			_scanThread->Kill();
		}
	}

	void MainWindow::CreateLayout()
	{
		this->SetSizeHints(wxDefaultSize, wxDefaultSize);

		///////////////////////////
		//BEGIN AUTO-GENERATED CODE

		_statusBar = this->CreateStatusBar(2, wxST_SIZEGRIP, wxID_ANY);
		wxBoxSizer* mainSizer;
		mainSizer = new wxBoxSizer(wxVERTICAL);

		_mainNotebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
		_generalPanel = new wxPanel(_mainNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
		_mainNotebook->AddPage(_generalPanel, wxT("General"), false);
		_aocTreePanel = new wxPanel(_mainNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
		wxBoxSizer* bSizer2;
		bSizer2 = new wxBoxSizer(wxVERTICAL);

		_aocTree = new wxTreeCtrl(_aocTreePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_FULL_ROW_HIGHLIGHT | wxTR_HIDE_ROOT);
		bSizer2->Add(_aocTree, 1, wxALL | wxEXPAND, 5);


		_aocTreePanel->SetSizer(bSizer2);
		_aocTreePanel->Layout();
		bSizer2->Fit(_aocTreePanel);
		_mainNotebook->AddPage(_aocTreePanel, wxT("Volumes"), true);

		mainSizer->Add(_mainNotebook, 1, wxEXPAND | wxALL, 5);


		//END AUTO-GENERATED CODE
		///////////////////////////

		_mainNotebook->Layout();

		this->SetSizer(mainSizer);
		this->Layout();

		this->Centre(wxBOTH);

	}

	void MainWindow::Initialize()
	{
		_aoHandler = new AOHandler(this);
		//_aoHandler->Scan();

		//Bind(AOHandler::WXE_AO_SCAN_UPDATE, &MainWindow::OnAOScanUpdate, this, AOHandler::WXE_AO_SCAN_UPDATE);
		//Connect(AOHandler::WXE_AO_SCAN_UPDATE, wxEventHandler(MainWindow::OnAOScanUpdate), NULL, this);

		_scanThread = new AOProcessingThread(this, _aoHandler);
		//_scanThread->Create();
		_scanThread->Run();


		//EVT_THREAD(AOHandler::WXE_AO_SCAN_UPDATE, MainWindow::OnAOScanUpdate)
		

		_aocTree->AddRoot("Volumes");
		auto treeRoot = _aocTree->GetRootItem();

		/*auto volumes = _aoHandler->GetAOCVolumes();
		for each (AOCVolume vol in volumes)
		{
			wxFileName name = vol.GetName();
			auto treeVolItem = _aocTree->AppendItem(treeRoot, name.GetFullName());

			auto aocFiles = vol.GetAOCFiles();
			for each (AOCFile file in aocFiles)
			{
				wxFileName name = file.GetName();
				auto treeAOCItem = _aocTree->AppendItem(treeVolItem, name.GetFullName());
			}
		}*/
	}

	void MainWindow::OnAOScanUpdate(wxThreadEvent& event)
	{
		auto aoUpdateEvent = static_cast<StatusUpdateEvent&>(event);

		_statusBar->SetStatusText(aoUpdateEvent.updateString, aoUpdateEvent.statusBarIndex);
	}
}