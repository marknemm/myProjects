#include "DataOptionsh.h"


DataOptions::KeyEdit::KeyEdit(){}


void DataOptions::KeyEdit::initialize(MorphWind *parent)
{
	if(parent == NULL) {
		debugBreak(DEBUG_LOC, "The argument *parent is NULL");
	}
	RECT dim = { 195, 387, 335, 414 };
	MorphWind::initialize("EDIT", _globHINST, NULL, (WS_CHILD | WS_VISIBLE | ES_READONLY | WS_DISABLED | ES_CENTER), dim, parent);
	HFONT hFont = CreateFont(25, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS
	  , CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Calibri"));
	sendMessage(WM_SETFONT, WPARAM(hFont), MAKELPARAM(TRUE, 0));
}


void DataOptions::KeyEdit::setText(int keyCode)
{
	std::string displayText("");
	if((keyCode > 0x40) && (keyCode < 0x5B)) {
		displayText += char(keyCode);
	}
	else if((keyCode > 0x2F) && (keyCode < 0x3A)) {
		displayText += char(keyCode);
	}
	else {
		switch(keyCode) {
			case VK_BACK:			displayText+="BACKSPACE";		break;
			case VK_TAB:			displayText+="TAB";				break;
			case VK_CLEAR:			displayText+="CLEAR";			break;
			case VK_RETURN:			displayText+="ENTER";			break;
			case VK_PAUSE:			displayText+="PAUSE";			break;
			case VK_CAPITAL:		displayText+="CAPS LOCK";		break;
			case VK_ESCAPE:			displayText+="ESC";				break;
			case VK_SPACE:			displayText+="SPACE";			break;
			case VK_PRIOR:			displayText+="PG UP";			break;
			case VK_NEXT:			displayText+="PG DN";			break;
			case VK_END:			displayText+="END";				break;
			case VK_HOME:			displayText+="HOME";			break;
			case VK_LEFT:			displayText+="LEFT ARROW";		break;
			case VK_UP:				displayText+="UP ARROW";		break;
			case VK_RIGHT:			displayText+="RIGHT ARROW";		break;
			case VK_DOWN:			displayText+="DOWN ARROW";		break;
			case VK_SELECT:			displayText+="SELECT";			break;
			case VK_PRINT:			displayText+="PRINT";			break;
			case VK_EXECUTE:		displayText+="EXECUTE";			break;
			case VK_SNAPSHOT:		displayText+="PRT SC";			break;
			case VK_INSERT:			displayText+="INS";				break;
			case VK_DELETE:			displayText+="DELETE";			break;
			case VK_HELP:			displayText+="HELP";			break;
			case VK_SLEEP:			displayText+="SLEEP";			break;
			case VK_NUMPAD0:		displayText+="NUM PAD 0";		break;
			case VK_NUMPAD1:		displayText+="NUM PAD 1";		break;
			case VK_NUMPAD2:		displayText+="NUM PAD 2";		break;
			case VK_NUMPAD3:		displayText+="NUM PAD 3";		break;
			case VK_NUMPAD4:		displayText+="NUM PAD 4";		break;
			case VK_NUMPAD5:		displayText+="NUM PAD 5";		break;
			case VK_NUMPAD6:		displayText+="NUM PAD 6";		break;
			case VK_NUMPAD7:		displayText+="NUM PAD 7";		break;
			case VK_NUMPAD8:		displayText+="NUM PAD 8";		break;
			case VK_NUMPAD9:		displayText+="NUM PAD 9";		break;
			case VK_MULTIPLY:		displayText+="MULTIPLY(*)";		break;
			case VK_ADD:			displayText+="PLUS(+)";			break;
			case VK_SEPARATOR:		displayText+="COLON(:)";		break;
			case VK_SUBTRACT:		displayText+="SUBTRACT(-)";		break;
			case VK_DECIMAL:		displayText+="DECIMAL(.)";		break;
			case VK_DIVIDE:			displayText+="DIVIDE(/)";		break;
			case VK_F1:				displayText+="F1";				break;
			case VK_F2:				displayText+="F2";				break;
			case VK_F3:				displayText+="F3";				break;
			case VK_F4:				displayText+="F4";				break;
			case VK_F5:				displayText+="F5";				break;
			case VK_F6:				displayText+="F6";				break;
			case VK_F7:				displayText+="F7";				break;
			case VK_F8:				displayText+="F8";				break;
			case VK_F9:				displayText+="F9";				break;
			case VK_F10:			displayText+="F10";				break;
			case VK_F11:			displayText+="F11";				break;
			case VK_F12:			displayText+="F12";				break;
			case VK_NUMLOCK:		displayText+="NUM LOCK";		break;
			case VK_SCROLL:			displayText+="SCROLL LOCK";		break;
			case VK_LSHIFT:			displayText+="LEFT SHIFT";		break;
			case VK_RSHIFT:			displayText+="RIGHT SHIFT";		break;
			case VK_LCONTROL:		displayText+="LEFT CTRL";		break;
			case VK_RCONTROL:		displayText+="RIGHT CTRL";		break;
			case VK_LMENU:			displayText+="LEFT ALT";		break;
			case VK_RMENU:			displayText+="RIGHT ALT";		break;
			case VK_VOLUME_MUTE:	displayText+="VOLUME MUTE";		break;
			case VK_VOLUME_DOWN:	displayText+="VOLUME DOWN";		break;
			case VK_VOLUME_UP:		displayText+="VOLUEM UP";		break;
			case VK_ZOOM:			displayText+="ZOOM";			break;
			case VK_SHIFT:			displayText+="SHIFT";			break;
			case VK_CONTROL:		displayText+="CTRL";			break;
			case VK_MENU:			displayText+="ALT";				break;
			case VK_OEM_1:			displayText+="; (or :)";		break;
			case VK_OEM_2:			displayText+="/ (or ?)";		break;
			case VK_OEM_3:			displayText+="` (or ~)";		break;
			case VK_OEM_4:			displayText+="[ (or {)";		break;
			case VK_OEM_5:			displayText+="\\ (or |)";		break;
			case VK_OEM_6:			displayText+="] (or })";		break;
			case VK_OEM_7:			displayText+="' (or QUOTE)";	break;
			case VK_OEM_COMMA:		displayText+=",  (or <)";		break;
			case VK_OEM_PERIOD:		displayText+=". (or >)";		break;
			case VK_OEM_PLUS:		displayText+="+ (or =)";		break;
		}
	}
	sendMessage(WM_SETTEXT, NULL, LPARAM(displayText.c_str()));
}