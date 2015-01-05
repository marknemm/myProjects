#include "DataOptionsh.h"


DataOptions::DataScroll::DataScroll(DataOptions *dataOptions)
: _order(ORDER_BY_INDEX),
  _deltaY(0),
  _highlightInd(-1),
  _maxDirective(-1),
  _selectedInd(),
  _prevMouseLoc(),
  _swapModeEnabled(false),
  _leftPress(false),
  _ctrlDown(false)
{
	_dataOptions = dataOptions;
	_prevMouseLoc.y = -1;
}


HWND DataOptions::DataScroll::getHWND() const
{
	return MorphWind::getHWND();
}


void DataOptions::DataScroll::orderByIndex()
{
	if(_order != ORDER_BY_INDEX) {
		RECT rcClient;
		_order = ORDER_BY_INDEX;
		_maxDirective = -1;
		_selectedInd.clear();
		getClientDim(&rcClient);
		RedrawWindow(getHWND(), &rcClient, NULL, RDW_INVALIDATE);
	}
}


void DataOptions::DataScroll::orderByAlpha()
{
	if(_order != ORDER_BY_ALPHA) {
		RECT rcClient;
		_order = ORDER_BY_ALPHA;
		_maxDirective = -1;
		_selectedInd.clear();
		getClientDim(&rcClient);
		RedrawWindow(getHWND(), &rcClient, NULL, RDW_INVALIDATE);
	}
}


void DataOptions::DataScroll::orderByTime()
{
	if(_order != ORDER_BY_TIME) {
		RECT rcClient;
		_order = ORDER_BY_TIME;
		_maxDirective = -1;
		_selectedInd.clear();
		getClientDim(&rcClient);
		RedrawWindow(getHWND(), &rcClient, NULL, RDW_INVALIDATE);
	}
}


int DataOptions::DataScroll::getNumDirectivesOfHighlight()
{
	int index(getRecordIndex(_highlightInd));
	return _recordHandler->getRecord(index)->numOfSubDirectives();
}


std::list<Record*>* DataOptions::DataScroll::getSelectedRecords()
{
	if(_selectedRecords.size() > 0) {
		return &_selectedRecords;
	}
	return NULL;
}


void DataOptions::DataScroll::enableSwapMode(bool enable)
{
	_swapModeEnabled = enable;
}


bool DataOptions::DataScroll::isSwapModeEnabled() const
{
	return _swapModeEnabled;
}


void DataOptions::DataScroll::paintRecords(Gdiplus::Graphics &graphics, RECT &rcClip, Gdiplus::Font &font)
{
	Record *record;
	std::stringstream ss;
	Gdiplus::Color black(0, 0, 0);
	int index(0);
	for(int r(_deltaY / 20) ; r < (_recordHandler->numOfRecords() - 1) && (r * 20) < rcClip.bottom ; r++) {
		index = getRecordIndex(r);
		record = _recordHandler->getRecord(index);
		ss = std::stringstream();
		ss << record->numIndex;
		std::string sstring(ss.str());
		std::wstring wstring(sstring.begin(), sstring.end());
		graphics.DrawString(wstring.c_str(), wstring.length(), &font, Gdiplus::PointF(Gdiplus::REAL(0), Gdiplus::REAL((r * 20) - rcClip.top)), &Gdiplus::SolidBrush(black));
		sstring = std::string(_recordHandler->getRecordTitle(index));
		wstring = std::wstring(sstring.begin(), sstring.end());
		graphics.DrawString(wstring.c_str(), wstring.length(), &font, Gdiplus::PointF(Gdiplus::REAL(80 - rcClip.left)
		  , Gdiplus::REAL((r * 20) - rcClip.top)), &Gdiplus::SolidBrush(black));
		sstring = std::string(_recordHandler->getRecordCreationDate(index) + "     " + _recordHandler->getRecordCreationTime(index));
		wstring = std::wstring(sstring.begin(), sstring.end());
		graphics.DrawString(wstring.c_str(), wstring.length(), &font, Gdiplus::PointF(Gdiplus::REAL(330 - rcClip.left)
		  , Gdiplus::REAL((r * 20) - rcClip.top)), &Gdiplus::SolidBrush(black));
	}
}


void DataOptions::DataScroll::paint()
{
	if(GetUpdateRect(getHWND(), NULL, FALSE) != 0) {
		PAINTSTRUCT ps;
		RECT rcClip;
		Gdiplus::Font font(L"Ariel", 14);
		Gdiplus::Color black(0, 0, 0);
		Gdiplus::Color dgray(100, 100, 100);
		HDC hdc = BeginPaint(getHWND(), &ps);
			
		POINT origin;
		GetWindowOrgEx(hdc, &origin);
		SetWindowOrgEx(hdc, origin.x, (origin.y + _deltaY), NULL);
		OffsetClipRgn(hdc, 0, _deltaY);
		GetClipBox(hdc, &rcClip);
	
		HDC hdcBuffer = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, (rcClip.right - rcClip.left), (rcClip.bottom - rcClip.top));
		SelectObject(hdcBuffer, hBitmap);
		Gdiplus::Graphics graphics(hdcBuffer);
	
		graphics.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(50, 200, 50)), 0, 0, (rcClip.right - rcClip.left), (rcClip.bottom - rcClip.top));
		if(_highlightInd > -1) {
			graphics.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(100, 200, 100)), 0, (((_highlightInd * 20) + 2) - rcClip.top), (rcClip.right - rcClip.left), 19);
		}
		int y;
		for(std::map<int, int>::iterator it(_selectedInd.begin()) ; it != _selectedInd.end() ; it++) {
			y = ((it->first * 20) + 2);
			if((y + 19) >= rcClip.top && y <= (rcClip.bottom)) {
				graphics.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(100, 255, 100)), 0, (y - rcClip.top), (rcClip.right - rcClip.left), 19);
			}
		}
		graphics.DrawLine(&Gdiplus::Pen(dgray, Gdiplus::REAL(1.0F)), Gdiplus::Point((80 - rcClip.left), 0), Gdiplus::Point((80 - rcClip.left), (rcClip.bottom - rcClip.top)));
		graphics.DrawLine(&Gdiplus::Pen(dgray, Gdiplus::REAL(1.0F)), Gdiplus::Point((330 - rcClip.left), 0), Gdiplus::Point((330 - rcClip.left), (rcClip.bottom - rcClip.top)));
		paintRecords(graphics, rcClip, font);
	
		BitBlt(hdc, rcClip.left, rcClip.top, (rcClip.right - rcClip.left), (rcClip.bottom - rcClip.top), hdcBuffer, 0, 0, SRCCOPY);
		SetWindowOrgEx(hdc, origin.x, origin.y, NULL);
		DeleteObject(hBitmap);
		DeleteObject(hdcBuffer);
		EndPaint(getHWND(), &ps);
	}
}

void DataOptions::DataScroll::scrollToHighlight()
{
	RECT rcClient;
	getClientDim(&rcClient);
	RECT updateRect = rcClient;
	GetCursorPos(&_prevMouseLoc);
	int oldDeltaY(_deltaY);
	if(((_highlightInd * 20) + 24) > (_deltaY + rcClient.bottom)) {
		_deltaY = ((_highlightInd * 20) + 24 - rcClient.bottom);
		ScrollWindowEx(getHWND(), 0, (oldDeltaY - _deltaY), NULL, NULL, NULL, &updateRect, SW_INVALIDATE);
		SetScrollPos(getHWND(), SB_VERT, _deltaY, TRUE);
	}
	else if((_highlightInd * 20) < _deltaY) {
		_deltaY = (_highlightInd * 20);
		ScrollWindowEx(getHWND(), 0, (oldDeltaY - _deltaY), NULL, NULL, NULL, &updateRect, SW_INVALIDATE);
		SetScrollPos(getHWND(), SB_VERT, _deltaY, TRUE);
	}
}


void DataOptions::DataScroll::selectAndScrollToRecords(std::list<Record*> &records)
{
	RECT rcClient;
	if(records.size() == 0) {
		debugBreak(DEBUG_LOC, "The parameter &records has a size of 0");
	}
	_selectedInd.clear();
	for(std::list<Record*>::iterator it(records.begin()) ; it != records.end() ; it++) {
		switch(_order) {
			case ORDER_BY_INDEX:	_selectedInd.insert(std::pair<int, int>(((*it)->numIndex - 1), ((*it)->numIndex - 1)));		break;
			case ORDER_BY_ALPHA:	_selectedInd.insert(std::pair<int, int>(((*it)->alphaIndex - 1), ((*it)->alphaIndex - 1)));	break;
			case ORDER_BY_TIME:		_selectedInd.insert(std::pair<int, int>(((*it)->dateIndex - 1), ((*it)->dateIndex - 1)));	break;
			default:				debugBreak(DEBUG_LOC, "The instance var. _order is not a defined value");
		}
		if((*it)->numOfSubDirectives() <= _maxDirective || _maxDirective < 0) {
			_maxDirective = ((*it)->numOfSubDirectives() - 1);
		}
	}
	_selectedRecords = records;
	GetCursorPos(&_prevMouseLoc);
	getClientDim(&rcClient);
	int oldDeltaY(_deltaY);
	if(((_selectedInd.begin()->first * 20) + 24) > (_deltaY + rcClient.bottom)) {
		_deltaY = ((_selectedInd.begin()->first * 20) + 24 - rcClient.bottom);
		HRGN updateRgn = CreateRectRgn(rcClient.left, (rcClient.top + _deltaY), rcClient.right, (rcClient.bottom + _deltaY));
		ScrollWindowEx(getHWND(), 0, (oldDeltaY - _deltaY), NULL, NULL, updateRgn, NULL, SW_INVALIDATE);
		SetScrollPos(getHWND(), SB_VERT, _deltaY, TRUE);
	}
	else if((_selectedInd.begin()->first * 20) < _deltaY) {
		_deltaY = (_selectedInd.begin()->first * 20);
		HRGN updateRgn = CreateRectRgn(rcClient.left, (rcClient.top + _deltaY), rcClient.right, (rcClient.bottom + _deltaY));
		ScrollWindowEx(getHWND(), 0, (oldDeltaY - _deltaY), NULL, NULL, updateRgn, NULL, SW_INVALIDATE);
		SetScrollPos(getHWND(), SB_VERT, _deltaY, TRUE);
	}
	redrawWindow(&rcClient, NULL, RDW_INVALIDATE);
	_dataOptions->selectRecord();
}


void DataOptions::DataScroll::vScroll(WPARAM wParam)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;
	GetScrollInfo(getHWND(), SB_VERT, &si);
	int iVertPos(si.nPos);
	switch(LOWORD(wParam)) {
		case SB_TOP:			si.nPos = si.nMin;			break;
		case SB_BOTTOM:			si.nPos = si.nMax;			break;
		case SB_LINEUP:			si.nPos -= 16;				break;
		case SB_LINEDOWN:		si.nPos += 16;				break;
		case SB_PAGEUP:			si.nPos -= si.nPage;		break;
		case SB_PAGEDOWN:		si.nPos += si.nPage;		break;
		case SB_THUMBTRACK:		si.nPos = si.nTrackPos;		break;        
	}
	si.fMask = SIF_POS;
	SetScrollInfo(getHWND(), SB_VERT, &si, TRUE);
	GetScrollInfo(getHWND(), SB_VERT, &si);
	if(si.nPos != iVertPos) {
		RECT rcUpdate;
		_deltaY += (si.nPos - iVertPos);
		getClientDim(&rcUpdate);
		HRGN updateRgn = CreateRectRgn(rcUpdate.left, (rcUpdate.top + _deltaY), rcUpdate.right, (rcUpdate.bottom + _deltaY));
	    ScrollWindowEx(getHWND(), 0, (iVertPos - si.nPos), NULL, NULL, updateRgn, NULL, SW_INVALIDATE);
	}
}


void DataOptions::DataScroll::adjustScrollInfo()
{
	SCROLLINFO si;
	GetScrollInfo(getHWND(), SB_VERT, &si);
	si.cbSize = sizeof(si);
	si.fMask  = (SIF_RANGE | SIF_PAGE);
	si.nMin = 0;
	si.nPage  = 200;
	si.nMax = int(_recordHandler->numOfRecords() * 20);
	if(si.nMax < 40) {
		si.nMax = 40;
	}
	SetScrollInfo(getHWND(), SB_VERT, &si, TRUE);
}


int DataOptions::DataScroll::getRecordIndex(int dataScrollIndex)
{
	switch(_order) {
		case ORDER_BY_INDEX:	return (dataScrollIndex + 1);
		case ORDER_BY_ALPHA:	return _recordHandler->getIndexFromAlphaOrder(dataScrollIndex + 1);
		case ORDER_BY_TIME:		return _recordHandler->getIndexFromDateOrder(dataScrollIndex + 1);
		default:
			std::stringstream ss;
			ss << "instance variable _order of value " << _order << " is incorrect";
			debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return -1;
}


void DataOptions::DataScroll::selectIndividualIndex()
{
	RECT updateRect, rcClient;
	HRGN updateRgn;
	int numDirectives(getNumDirectivesOfHighlight());
	getClientDim(&rcClient);
	updateRect = rcClient;
	updateRect.top = ((_highlightInd * 20) - _deltaY);
	updateRect.bottom = (updateRect.top + 21);
	updateRgn = CreateRectRgnIndirect(&updateRect);
	if(!_ctrlDown) {
		HRGN updateRgn2;
		int y;
		for(std::map<int, int>::iterator it(_selectedInd.begin()) ; it != _selectedInd.end() ; it++) {
			y = (it->first * 20);
			if(y <= (rcClient.bottom + _deltaY) && (y + 21) >= rcClient.top) {
				updateRect.top = (y - _deltaY);
				updateRect.bottom = (updateRect.top + 21);
				updateRgn2 = CreateRectRgnIndirect(&updateRect);
				CombineRgn(updateRgn, updateRgn, updateRgn2, RGN_OR);
			}
		}
		_selectedInd.clear();
		_selectedRecords.clear();
		_maxDirective = (numDirectives - 1);
	}
	else if(numDirectives <= _maxDirective) {
		_maxDirective = (numDirectives - 1);
	}
	_selectedInd.insert(std::pair<int, int>(_highlightInd, _highlightInd));
	_selectedRecords.push_back(_recordHandler->getRecord(getRecordIndex(_highlightInd)));
	_dataOptions->selectRecord();
	if(!_swapModeEnabled)
		redrawWindow(NULL, updateRgn, RDW_INVALIDATE);
}


void DataOptions::DataScroll::create()
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask  = SIF_PAGE;
	si.nPage  = 200;
	SetScrollInfo(getHWND(), SB_VERT, &si, TRUE);
}


void DataOptions::DataScroll::mouseMove(LPARAM lParam)
{
	int y, oldHighlightInd, newHighlightInd;
	POINT moveCheckPoint;
	if(_dataOptions->_highlightHeader != 0) {
		RECT updateRect = { 41, 62, 609, 85 };
		_dataOptions->_highlightHeader = 0;
		_dataOptions->redrawWindow(&updateRect, NULL, RDW_INVALIDATE);
	}
	GetCursorPos(&moveCheckPoint);
	y = GET_Y_LPARAM(lParam);
	newHighlightInd = ((y + _deltaY)  / 20);
	if(moveCheckPoint.y != _prevMouseLoc.y && newHighlightInd < (_recordHandler->numOfRecords() - 1)) {
		if(newHighlightInd < 0 || newHighlightInd >= _recordHandler->numOfRecords()) {
			std::stringstream ss;
			ss << "The new highlight index " << newHighlightInd << " is outside of the range of record indicees";
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
		oldHighlightInd = _highlightInd;
		_highlightInd = newHighlightInd;
		if(oldHighlightInd != _highlightInd) {
			if(_leftPress) {
				int numDirectives(getNumDirectivesOfHighlight());
				if(numDirectives <= _maxDirective) {
					_maxDirective = (numDirectives - 1);
				}
				_selectedInd.insert(std::pair<int, int>(_highlightInd, _highlightInd));
				_selectedRecords.push_back(_recordHandler->getRecord(getRecordIndex(_highlightInd)));
			}
			RECT updateRect;
			getClientDim(&updateRect);
			updateRect.top = ((newHighlightInd * 20) - _deltaY);
			updateRect.bottom = (updateRect.top + 21);
			HRGN updateRgn = CreateRectRgnIndirect(&updateRect);
			if(oldHighlightInd != -1) {
				updateRect.top = ((oldHighlightInd * 20) - _deltaY);
				updateRect.bottom = (updateRect.top + 21);
				HRGN updateRgn2 = CreateRectRgnIndirect(&updateRect);
				CombineRgn(updateRgn, updateRgn, updateRgn2, RGN_OR);
			}
			GetRgnBox(updateRgn, &updateRect);
			redrawWindow(NULL, updateRgn, RDW_INVALIDATE);
			if(_leftPress) {
				_dataOptions->selectRecord();
			}
		}
		GetCursorPos(&_prevMouseLoc);
	}
}


void DataOptions::DataScroll::leftButtonDown()
{
	_leftPress = true;
	if(_highlightInd > -1 && (_selectedInd.find(_highlightInd) == _selectedInd.end() || (_selectedInd.size() > 1 && _ctrlDown == false))) {
		if(_swapModeEnabled) {
			HRGN updateRgn;
			RECT clientRect, updateRect;
			getClientDim(&clientRect);
			updateRect = clientRect;
			if(getSelectedRecords() == NULL) {
				debugBreak(DEBUG_LOC, "Selected records is NULL with swap mode enabled");
			}
			int origSelectRecordInd(getSelectedRecords()->front()->numIndex); //For record handler index
			int origSelectInd(_selectedInd.begin()->first); //For data scroll index
			selectIndividualIndex();
			_recordHandler->swapRecords(origSelectRecordInd, getSelectedRecords()->front()->numIndex);
			updateRect.top = (_selectedInd.begin()->first * 20);
			updateRect.bottom = (updateRect.top + 24);
			updateRgn = CreateRectRgnIndirect(&updateRect);
			int origSelectedIndY(origSelectInd * 20);
			if(origSelectedIndY <= (_deltaY + clientRect.bottom) && (origSelectedIndY + 24) >= _deltaY) {
				updateRect.top = (origSelectedIndY - _deltaY);
				updateRect.bottom = ((origSelectedIndY + 24) - _deltaY);
				HRGN updateRgn2 = CreateRectRgnIndirect(&updateRect);
				CombineRgn(updateRgn, updateRgn, updateRgn2, RGN_OR);
			}
			redrawWindow(NULL, updateRgn, RDW_INVALIDATE);
			enableSwapMode(false);
			RECT updateRectParent = { 40, 45, 200, 60 };
			getParent().redrawWindow(&updateRectParent, NULL, RDW_INVALIDATE);
		}
		else {  selectIndividualIndex();  }
	}
}


void DataOptions::DataScroll::leftButtonUp()
{
	_leftPress = false;
}


void DataOptions::DataScroll::mouseWheel(WPARAM wParam, LPARAM lParam)
{
	RECT dataScrollPos;
	int x(GET_X_LPARAM(lParam));
	int y(GET_Y_LPARAM(lParam));
	getDim(&dataScrollPos);
	if(x > dataScrollPos.left && x < dataScrollPos.right && y > dataScrollPos.top && y < dataScrollPos.bottom) {
		short wheelDelta(GET_WHEEL_DELTA_WPARAM(wParam));
		if(wheelDelta > 0) {
			vScroll(MAKEWPARAM(SB_LINEUP, NULL));
		}
		else {  vScroll(MAKEWPARAM(SB_LINEDOWN, NULL));  }
	}
}


void DataOptions::DataScroll::keyDown(WPARAM wParam)
{
	RECT clientRect, updateRect;
	getClientDim(&clientRect);
	updateRect = clientRect;
	switch(wParam) {
		case VK_DOWN:
			if((_highlightInd + 1) < (_recordHandler->numOfRecords() - 1)) {
				_highlightInd++;
				scrollToHighlight();
				getClientDim(&updateRect);
				updateRect.top = (((_highlightInd - 1) * 20) - _deltaY);
				updateRect.bottom = (updateRect.top + 44);
				redrawWindow(&updateRect, NULL, RDW_INVALIDATE);
			}
			break;
		case VK_UP:
			if((_highlightInd - 1) >= 0) {
				_highlightInd--;
				scrollToHighlight();
				getClientDim(&updateRect);
				updateRect.top = ((_highlightInd * 20) - _deltaY);
				updateRect.bottom = (updateRect.top + 44);
				redrawWindow(&updateRect, NULL, RDW_INVALIDATE);
			}
			break;
		case VK_RETURN:
			if((_selectedInd.find(_highlightInd) == _selectedInd.end() || (_selectedInd.size() > 1 && _ctrlDown == false)) && _highlightInd > -1) {
				selectIndividualIndex();
			}
			break;
		case VK_DELETE:
			if(_selectedInd.size() > 0) {
				_dataOptions->sendMessage((WM_APP + DELETE_RECORD_BUTTON_ID), NULL, NULL);
			}
			break;
		case VK_CONTROL:
			_ctrlDown = true;
			break;
		default:	break;
	}
}


void DataOptions::DataScroll::keyUp(WPARAM wParam)
{
	if(wParam == VK_CONTROL)
		_ctrlDown = false;
}


LRESULT CALLBACK DataOptions::DataScroll::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_CREATE:			create();					break;
		case WM_PAINT:			paint();					break;
		case WM_VSCROLL:		vScroll(wParam);			break;
		case WM_MOUSEMOVE:		mouseMove(lParam);			break;
		case WM_LBUTTONDOWN:	leftButtonDown();			break;
		case WM_LBUTTONUP:		leftButtonUp();				break;
		case WM_MOUSEWHEEL:		mouseWheel(wParam, lParam);	return 0;
		case WM_KEYDOWN:		keyDown(wParam);			break;
		case WM_KEYUP:			keyUp(wParam);				break;
		case WM_ERASEBKGND:									return 0;
		default:											break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}