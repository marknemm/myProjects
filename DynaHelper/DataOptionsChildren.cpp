#include "DataOptionsh.h"


void DataOptions::createButtons()
{
	HWND hwnd;
	//setTitleButton, setDelayButton, setLocationButton, swapRecordsButton, deleteRecordsButton, deleteAllRecordsButton, saveAndExitButton, exitButton;
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	UINT style = (WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_DEFPUSHBUTTON | BS_TEXT | BS_MULTILINE | BS_VCENTER);

	hwnd = CreateWindowEx(NULL, "BUTTON", "Set Title", style, 530, 295, 150, 35, this->getHWND(), (HMENU)SET_TITLE_BUTTON_ID, _globHINST, NULL);
	_buttons[SET_TITLE_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Set Delay", style, 530, 340, 150, 35, this->getHWND(), (HMENU)SET_DELAY_BUTTON_ID, _globHINST, NULL);
	_buttons[SET_DELAY_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Set Location", style, 40, 340, 150, 35, this->getHWND(), (HMENU)SET_LOCATION_BUTTON_ID, _globHINST, NULL);
	_buttons[SET_LOCATION_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Set Key", style, 40, 385, 150, 35, this->getHWND(), (HMENU)SET_KEY_BUTTON_ID, _globHINST, NULL);
	_buttons[SET_KEY_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Set Duration", style, 530, 385, 150, 35, this->getHWND(), (HMENU)SET_DURATION_BUTTON_ID, _globHINST, NULL);
	_buttons[SET_DURATION_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Set Text", style, 40, 430, 150, 35, this->getHWND(), (HMENU)SET_TEXT_BUTTON_ID, _globHINST, NULL);
	_buttons[SET_TEXT_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Set Scrolls", style, 40, 475, 150, 35, this->getHWND(), (HMENU)SET_SCROLLS_BUTTON_ID, _globHINST, NULL);
	_buttons[SET_SCROLLS_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Duplicate Record", style, 325, 475, 230, 35, this->getHWND(), (HMENU)DUPLICATE_RECORD_BUTTON_ID, _globHINST, NULL);
	_buttons[DUPLICATE_RECORD_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Add Record", (style & ~WS_DISABLED), 610, 475, 230, 35, this->getHWND(), (HMENU)ADD_RECORD_BUTTON_ID, _globHINST, NULL);
	_buttons[ADD_RECORD_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "ADD", style, 770, 112, 35, 162, this->getHWND(), (HMENU)ADD_DIRECTIVE_BUTTON_ID, _globHINST, NULL);
	_buttons[ADD_DIRECTIVE_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Swap Records", style, 40, 520, 230, 35, this->getHWND(), (HMENU)SWAP_RECORDS_BUTTON_ID, _globHINST, NULL);
	_buttons[SWAP_RECORDS_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Delete Record", style, 325, 520, 230, 35, this->getHWND(), (HMENU)DELETE_RECORD_BUTTON_ID, _globHINST, NULL);
	_buttons[DELETE_RECORD_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "DE L", style, 815, 112, 35, 162, this->getHWND(), (HMENU)DELETE_DIRECTIVE_BUTTON_ID, _globHINST, NULL);
	_buttons[DELETE_DIRECTIVE_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	UINT specStyle = style;
	if(_recordHandler->numOfRecords() > 0) {
		specStyle = (style & ~WS_DISABLED);
	}
	hwnd = CreateWindowEx(NULL, "BUTTON", "Delete All Records", specStyle, 610, 520, 230, 35, this->getHWND(), (HMENU)DELETE_ALl_RECORDS_BUTTON_ID, _globHINST, NULL);
	_buttons[DELETE_ALl_RECORDS_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Save and Exit", (style & ~WS_DISABLED), 140, 565, 225, 40, this->getHWND(), (HMENU)SAVE_AND_EXIT_BUTTON_ID, _globHINST, NULL);
	_buttons[SAVE_AND_EXIT_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	hwnd = CreateWindowEx(NULL, "BUTTON", "Exit", (style & ~WS_DISABLED), 520, 565, 225, 40, this->getHWND(), (HMENU)EXIT_BUTTON_ID, _globHINST, NULL);
	_buttons[EXIT_BUTTON_ID] = MorphWind(hwnd, _globHINST, this, "BUTTON");

	for(int w(0) ; w < SAVE_AND_EXIT_BUTTON_ID ; w++) {
		_buttons[w].sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	}
	hFont = CreateFont(30, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	_buttons[SAVE_AND_EXIT_BUTTON_ID].sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	_buttons[EXIT_BUTTON_ID].sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
}


void DataOptions::createEdits()
{
	_titleEdit.initialize(this);
	_delayEdit.initialize(this, false);
	_locationEdit[0].initialize(this, LOC_1);
	_locationEdit[1].initialize(this, LOC_2);
	_keyEdit.initialize(this);
	_durationEdit.initialize(this, true);
	_textEdit.initialize(this);
	_scrollEdit.initialize(this);
}


void DataOptions::initActionDrop()
{
	HFONT hFont;
	UINT style = (CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DISABLED);
	_actionDropWind = MorphWind(CreateWindowEx(NULL, WC_COMBOBOX, "", style, 40, 295, 250, 320, getHWND(), NULL, _globHINST, NULL), _globHINST, this, WC_COMBOBOX);
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Left Click"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Left Press"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Right Click"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Double Click"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Scroll Up"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("scroll Down"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Click & Drag"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Text Input"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Key Type"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Key Press"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Relative Mouse Action"));
	_actionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("No Action"));
	hFont = CreateFont(25, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Calibri"));
	_actionDropWind.sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(FALSE, 0));
}


void DataOptions::initRelativeActionDrop()
{
	HFONT hFont;
	UINT style = (CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DISABLED);
	_relativeActionDropWind = MorphWind(CreateWindowEx(NULL, WC_COMBOBOX, "", style, 295, 295, 200, 220, getHWND(), NULL, _globHINST, NULL), _globHINST, this, WC_COMBOBOX);
	_relativeActionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Rel. Left Click"));
	_relativeActionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Rel. Left Press"));
	_relativeActionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Rel. Right Click"));
	_relativeActionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Rel. Double Click"));
	_relativeActionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Rel. Scroll Up"));
	_relativeActionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Rel. scroll Down"));
	_relativeActionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Rel. Click & Drag"));
	_relativeActionDropWind.sendMessage(CB_ADDSTRING, NULL, LPARAM("Rel. No Action"));
	hFont = CreateFont(25, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Calibri"));
	_relativeActionDropWind.sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(FALSE, 0));
}