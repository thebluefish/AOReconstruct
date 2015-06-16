#include "MainWindow.h"

#include "AOHandler.h"
#include "Utils.h"

namespace AOReconstruct
{
	wxDEFINE_EVENT(wxEVT_STATUS_UPDATE, wxThreadEvent);

	wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
		EVT_THREAD(wxEVT_STATUS_UPDATE, MainWindow::OnAOScanUpdate)
		EVT_THREAD(wxEVT_AOC_SCAN_FINISHED, MainWindow::OnAOScanFinished)
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
		_pickerPST->Disconnect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(MainWindow::_pickerPSTOnFileChanged), NULL, this);

		EndScanThreadSafely();
	}

	void MainWindow::EndScanThreadSafely()
	{
		{
			wxCriticalSectionLocker enter(_scanThreadCS);

			if (_scanThread)
			{
				_scanThread->Delete();
			}
		}

		while (1)
		{
			{ // was the ~MyThread() function executed?
				wxCriticalSectionLocker enter(_scanThreadCS);
				if (!_scanThread) break;
			}
			// wait for thread completion
			wxThread::This()->Sleep(1);
		}
	}

	void MainWindow::CreateLayout()
	{
		this->SetSizeHints(wxDefaultSize, wxDefaultSize);
		this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

		///////////////////////////
		//BEGIN AUTO-GENERATED CODE

		_statusBar = this->CreateStatusBar(2, wxST_SIZEGRIP, wxID_ANY);
		wxBoxSizer* mainSizer;
		mainSizer = new wxBoxSizer(wxVERTICAL);

		_labelSelectPST = new wxStaticText(this, wxID_ANY, wxT("Select PST to rebuild:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
		_labelSelectPST->Wrap(-1);
		mainSizer->Add(_labelSelectPST, 0, wxALL, 3);

		_pickerPST = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, wxT("Select a PST file"), wxT("*.pst"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE | wxFLP_FILE_MUST_EXIST | wxFLP_OPEN);
		mainSizer->Add(_pickerPST, 0, wxALL | wxEXPAND, 3);

		_lineSeparator = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
		mainSizer->Add(_lineSeparator, 0, wxEXPAND | wxALL, 5);

		wxBoxSizer* secondarySizer;
		secondarySizer = new wxBoxSizer(wxHORIZONTAL);

		_aocTree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_FULL_ROW_HIGHLIGHT | wxTR_HIDE_ROOT);
		secondarySizer->Add(_aocTree, 1, wxALL | wxEXPAND, 3);

		_panelOptions = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
		wxStaticBoxSizer* sizerOptions;
		sizerOptions = new wxStaticBoxSizer(new wxStaticBox(_panelOptions, wxID_ANY, wxT("Options")), wxVERTICAL);

		m_checkBox1 = new wxCheckBox(_panelOptions, wxID_ANY, wxT("Backup Archive"), wxDefaultPosition, wxDefaultSize, 0);
		sizerOptions->Add(m_checkBox1, 0, wxALL, 5);


		sizerOptions->Add(0, 0, 1, wxEXPAND, 5);

		_buttonRebuild = new wxButton(_panelOptions, wxID_ANY, wxT("Rebuild"), wxDefaultPosition, wxDefaultSize, 0);
		sizerOptions->Add(_buttonRebuild, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT, 3);


		_panelOptions->SetSizer(sizerOptions);
		_panelOptions->Layout();
		sizerOptions->Fit(_panelOptions);
		secondarySizer->Add(_panelOptions, 1, wxEXPAND | wxALL, 1);


		mainSizer->Add(secondarySizer, 1, wxEXPAND, 5);


		//END AUTO-GENERATED CODE
		///////////////////////////

		_aocTree->AddRoot("Volumes");

		this->SetSizer(mainSizer);
		this->Layout();

		this->Centre(wxBOTH);

		_pickerPST->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(MainWindow::_pickerPSTOnFileChanged), NULL, this);
		_aocTree->Connect(wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(MainWindow::_aocTreeOnTreeItemExpanding), NULL, this);
	}

	void MainWindow::Initialize()
	{
		_aoHandler = std::make_shared<AOHandler>(this);
		
	}

	void MainWindow::_pickerPSTOnFileChanged(wxFileDirPickerEvent& event)
	{
		EndScanThreadSafely();

		wxFileName pstFileName = event.GetPath();

		// Clear the TreeView
		auto treeRoot = _aocTree->GetRootItem();
		_aocTree->DeleteChildren(treeRoot);

		_scanThread = new AOScanningThread(pstFileName, this, _aoHandler.get());
		_scanThread->Run();

		std::string path(pstFileName.GetFullPath());
		std::string password("skauf1skauf1@NIKEskauf1_Cache.pst");
		std::string storeName("skauf1_Cache");

		//_pstHandler = std::make_shared<PSTHandler>(path, password, Utils::string2wstring(storeName));
	}

	void MainWindow::_aocTreeOnTreeItemExpanding(wxTreeEvent& event)
	{
		auto item = event.GetItem();
		auto label = _aocTree->GetItemText(item);

		auto treeRoot = _aocTree->GetRootItem();

		auto itemData = static_cast<AOCTreeItemData*>(_aocTree->GetItemData(item));

		// Don't populate list if this volume has already been populated
		if (itemData->hasBeenPopulated)
			return;

		auto& volumes = _aoHandler->GetAOCVolumes();
		for each (AOCVolume vol in volumes)
		{
			wxFileName name = vol.GetName();

			if (name.GetFullName() == label)
			{
				// Populate expanded volume
				auto& aocFiles = vol.GetAOCFiles();
				for each (AOCFile file in aocFiles)
				{
					wxFileName name = file.GetName();
					auto treeAOCItem = _aocTree->AppendItem(item, name.GetFullName());
				}

				break;
			}
		}

		itemData->hasBeenPopulated = true;
	}

	void MainWindow::OnAOScanUpdate(wxThreadEvent& event)
	{
		auto aoUpdateEvent = static_cast<StatusUpdateEvent&>(event);

		_statusBar->SetStatusText(aoUpdateEvent.updateString, aoUpdateEvent.statusBarIndex);
	}

	void MainWindow::OnAOScanFinished(wxThreadEvent& event)
	{
		//EndScanThreadSafely();

		_statusBar->SetStatusText("Parsing data", 0);
		_statusBar->SetStatusText("Updating...", 1);

		auto treeRoot = _aocTree->GetRootItem();

		auto& volumes = _aoHandler->GetAOCVolumes();
		for each (AOCVolume vol in volumes)
		{
			wxFileName name = vol.GetName();
			auto treeVolItem = _aocTree->AppendItem(treeRoot, name.GetFullName());
			_aocTree->SetItemData(treeVolItem, new AOCTreeItemData());

			if (vol.GetAOCFiles().size() > 0)
			{
				_aocTree->SetItemHasChildren(treeVolItem, true);
			}
			//for each (AOCFile file in aocFiles)
			//{
			//	wxFileName name = file.GetName();
			//	auto treeAOCItem = _aocTree->AppendItem(treeVolItem, name.GetFullName());
			//}
		}

		_statusBar->SetStatusText("Finished Scan", 0);
		_statusBar->SetStatusText("", 1);
	}
}