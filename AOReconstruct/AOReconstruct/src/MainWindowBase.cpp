///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  5 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MainWindowBase.h"

///////////////////////////////////////////////////////////////////////////

MainWindowBase::MainWindowBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	_statusBar = this->CreateStatusBar(2, wxST_SIZEGRIP, wxID_ANY);
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer(wxVERTICAL);

	m_notebook1 = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	_reconstituteTab = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer(wxVERTICAL);

	_labelStep1 = new wxStaticText(_reconstituteTab, wxID_ANY, wxT("1. Select target PST file (NOT a Local Archive):"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	_labelStep1->Wrap(-1);
	bSizer41->Add(_labelStep1, 0, wxALL, 3);

	_newPSTPicker = new wxFilePickerCtrl(_reconstituteTab, wxID_ANY, wxEmptyString, wxT("Select a PST file"), wxT("*.pst"), wxDefaultPosition, wxDefaultSize, wxFLP_SAVE | wxFLP_USE_TEXTCTRL);
	bSizer41->Add(_newPSTPicker, 0, wxALL | wxEXPAND, 3);

	_lineSeparator1 = new wxStaticLine(_reconstituteTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer41->Add(_lineSeparator1, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer(wxVERTICAL);

	_labelStep2 = new wxStaticText(_reconstituteTab, wxID_ANY, wxT("2. Select Archives to merge:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	_labelStep2->Wrap(-1);
	bSizer17->Add(_labelStep2, 0, wxALL, 5);

	wxArrayString _archivesCheckListChoices;
	_archivesCheckList = new wxCheckListBox(_reconstituteTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, _archivesCheckListChoices, wxLB_ALWAYS_SB | wxLB_MULTIPLE | wxLB_SORT);
	bSizer17->Add(_archivesCheckList, 1, wxALL | wxEXPAND, 5);


	bSizer41->Add(bSizer17, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer(wxHORIZONTAL);

	_labelStep3 = new wxStaticText(_reconstituteTab, wxID_ANY, wxT("3. Reconstitute archives into PST:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	_labelStep3->Wrap(-1);
	bSizer35->Add(_labelStep3, 1, wxALL, 5);

	_disableAxsOneButton = new wxButton(_reconstituteTab, wxID_ANY, wxT("Disable AxsOne"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer35->Add(_disableAxsOneButton, 0, wxALL, 5);


	bSizer6->Add(bSizer35, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer(wxHORIZONTAL);

	_scanGauge = new wxGauge(_reconstituteTab, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
	_scanGauge->SetValue(0);
	bSizer18->Add(_scanGauge, 1, wxALIGN_CENTER | wxALL | wxEXPAND, 5);

	_reconstituteButton = new wxButton(_reconstituteTab, wxID_ANY, wxT("Reconstitute"), wxDefaultPosition, wxDefaultSize, 0);
	_reconstituteButton->Enable(false);

	bSizer18->Add(_reconstituteButton, 0, wxALIGN_CENTER | wxALL, 5);


	bSizer6->Add(bSizer18, 1, wxEXPAND, 5);


	bSizer41->Add(bSizer6, 0, wxEXPAND, 5);


	_reconstituteTab->SetSizer(bSizer41);
	_reconstituteTab->Layout();
	bSizer41->Fit(_reconstituteTab);
	m_notebook1->AddPage(_reconstituteTab, wxT("Reconstitute"), true);
	_rebuildTab = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxVERTICAL);

	_labelSelectPST = new wxStaticText(_rebuildTab, wxID_ANY, wxT("Select PST to rebuild:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	_labelSelectPST->Wrap(-1);
	bSizer4->Add(_labelSelectPST, 0, wxALL, 3);

	_pickerPST = new wxFilePickerCtrl(_rebuildTab, wxID_ANY, wxEmptyString, wxT("Select a PST file"), wxT("*.pst"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE | wxFLP_FILE_MUST_EXIST | wxFLP_OPEN);
	bSizer4->Add(_pickerPST, 0, wxALL | wxEXPAND, 3);

	_lineSeparator = new wxStaticLine(_rebuildTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer4->Add(_lineSeparator, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* secondarySizer;
	secondarySizer = new wxBoxSizer(wxHORIZONTAL);

	_aocTree = new wxTreeCtrl(_rebuildTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_FULL_ROW_HIGHLIGHT | wxTR_HIDE_ROOT);
	secondarySizer->Add(_aocTree, 1, wxALL | wxEXPAND, 3);

	_panelOptions = new wxPanel(_rebuildTab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sizerOptions;
	sizerOptions = new wxStaticBoxSizer(new wxStaticBox(_panelOptions, wxID_ANY, wxT("Options")), wxVERTICAL);

	_backupArchiveCheckbox = new wxCheckBox(_panelOptions, wxID_ANY, wxT("Backup Archive"), wxDefaultPosition, wxDefaultSize, 0);
	_backupArchiveCheckbox->Enable(false);

	sizerOptions->Add(_backupArchiveCheckbox, 0, wxALL, 5);

	_disableAxsOneCheckbox = new wxCheckBox(_panelOptions, wxID_ANY, wxT("Disable AxsOne"), wxDefaultPosition, wxDefaultSize, 0);
	_disableAxsOneCheckbox->SetValue(true);
	_disableAxsOneCheckbox->Enable(false);

	sizerOptions->Add(_disableAxsOneCheckbox, 0, wxALL, 5);


	sizerOptions->Add(0, 0, 1, wxEXPAND, 5);


	_panelOptions->SetSizer(sizerOptions);
	_panelOptions->Layout();
	sizerOptions->Fit(_panelOptions);
	secondarySizer->Add(_panelOptions, 1, wxEXPAND | wxALL, 1);


	bSizer4->Add(secondarySizer, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer(wxHORIZONTAL);

	_rebuildGauge = new wxGauge(_rebuildTab, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
	_rebuildGauge->SetValue(0);
	bSizer61->Add(_rebuildGauge, 1, wxALIGN_CENTER | wxALL | wxEXPAND, 5);

	_buttonRebuild = new wxButton(_rebuildTab, wxID_ANY, wxT("Rebuild"), wxDefaultPosition, wxDefaultSize, 0);
	_buttonRebuild->Enable(false);

	bSizer61->Add(_buttonRebuild, 0, wxALIGN_CENTER | wxALL, 5);


	bSizer4->Add(bSizer61, 0, wxEXPAND, 5);


	_rebuildTab->SetSizer(bSizer4);
	_rebuildTab->Layout();
	bSizer4->Fit(_rebuildTab);
	m_notebook1->AddPage(_rebuildTab, wxT("Rebuild"), false);
	_logTab = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer(wxVERTICAL);

	_logTextbox = new wxTextCtrl(_logTab, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_URL | wxTE_DONTWRAP | wxTE_MULTILINE | wxTE_RICH);
	bSizer16->Add(_logTextbox, 1, wxALL | wxEXPAND, 5);


	_logTab->SetSizer(bSizer16);
	_logTab->Layout();
	bSizer16->Fit(_logTab);
	m_notebook1->AddPage(_logTab, wxT("Log"), false);

	mainSizer->Add(m_notebook1, 1, wxEXPAND | wxALL, 5);


	this->SetSizer(mainSizer);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	_newPSTPicker->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(MainWindowBase::_newPSTPickerOnFileChanged), NULL, this);
	_disableAxsOneButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainWindowBase::_disableAxsOneButtonOnButtonClick), NULL, this);
	_reconstituteButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainWindowBase::_reconstituteButtonOnButtonClick), NULL, this);
	_pickerPST->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(MainWindowBase::_pickerPSTOnFileChanged), NULL, this);
	_buttonRebuild->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainWindowBase::_buttonRebuildOnButtonClick), NULL, this);
}

MainWindowBase::~MainWindowBase()
{
	// Disconnect Events
	_newPSTPicker->Disconnect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(MainWindowBase::_newPSTPickerOnFileChanged), NULL, this);
	_disableAxsOneButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainWindowBase::_disableAxsOneButtonOnButtonClick), NULL, this);
	_reconstituteButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainWindowBase::_reconstituteButtonOnButtonClick), NULL, this);
	_pickerPST->Disconnect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(MainWindowBase::_pickerPSTOnFileChanged), NULL, this);
	_buttonRebuild->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainWindowBase::_buttonRebuildOnButtonClick), NULL, this);

}
