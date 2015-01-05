#include "DataOptionsh.h"


DataOptions::LocationEdit::LocationEdit(){}


void DataOptions::LocationEdit::initialize(MorphWind *parent, int loc)
{
	if(parent == NULL) {
		debugBreak(DEBUG_LOC, "The argument *parent is NULL");
	}
	if(loc > 2 || loc < 0) {
		std::stringstream ss;
		ss << "The argument loc of value " << loc << " is an incorrect value";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	RECT dim = { 195, 343, 345, 370 };
	if(loc == 2) {
		dim.left = 348;
		dim.right = 496;
	}
	MorphWind::initialize("EDIT", _globHINST, NULL, (WS_CHILD | WS_VISIBLE | ES_READONLY | WS_DISABLED | ES_CENTER), dim, parent);
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Calibri"));
	sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
}