#include "DataOptionsh.h"


DataOptions::DirectiveWind::DirectiveWind()
:  _directive(0)
{}


void DataOptions::DirectiveWind::initialize(DataOptions *parent)
{
	if(parent == NULL) {
		debugBreak(DEBUG_LOC, "The argument *parent is NULL");
	}
	_dataOptions = parent;
	RECT dim = { 641, 150, 760, 225 };
	MorphWind::initialize("EDIT", _globHINST, NULL, (WS_CHILD | WS_DISABLED | WS_VISIBLE | ES_CENTER | ES_READONLY), dim, parent);
	HFONT hFont = CreateFont(80, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
	sendMessage(WM_SETTEXT, NULL, LPARAM("00"));
}


int DataOptions::DirectiveWind::getDirective() const
{
	return _directive;
}


void DataOptions::DirectiveWind::setDirective(int directive)
{
	std::stringstream ss;
	if(directive < 0 || (_dataOptions->getSelectedRecords() == NULL && directive > 0)) {
		ss << "The directive index " << directive << " is outside of the bounds of the number of sub-directives for the selected record";
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_directive = directive;
	ss = std::stringstream();
	if(directive < 10) {
		ss << '0';
	}
	ss << directive;
	sendMessage(WM_SETTEXT, NULL, LPARAM(ss.str().c_str()));
}