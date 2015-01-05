#include "MainMenuPane.h"
#include "MainMenuText.h"


MainMenuPane::MainMenuPane(MainMenuFrame *menuFrame)
: _highlightIndex(0),
  _prevHighlightIndex(0),
  _deltaY(0),
  _search(""),
  _height(),
  _prevMouseLoc(),
  _bounds(),
  _indices()
{
	_menuFrame = menuFrame;
}


void MainMenuPane::paint()
{
	using namespace Gdiplus;

	if(GetUpdateRect(getHWND(), NULL, FALSE) != 0) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(getHWND(), &ps);
		
		POINT origin;
		GetWindowOrgEx(hdc, &origin);
		SetWindowOrgEx(hdc, origin.x, (origin.y + _deltaY), NULL);
		OffsetRect(&ps.rcPaint, 0, _deltaY);
	
		HDC hdcBuffer = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, (ps.rcPaint.right - ps.rcPaint.left), (ps.rcPaint.bottom - ps.rcPaint.top));
		SelectObject(hdcBuffer, hBitmap);
		
		Graphics graphics(hdcBuffer);
		GraphicsPath path;
		path.AddRectangle(RectF(REAL(0), REAL(0), REAL(ps.rcPaint.right - ps.rcPaint.left), REAL(ps.rcPaint.bottom - ps.rcPaint.top)));
		PathGradientBrush pathGradBrush(&path);
		pathGradBrush.SetGammaCorrection(TRUE);
		Color bClr(255, 200, 200, 255);
		int numB(1);
		pathGradBrush.SetSurroundColors(&bClr, &numB);
		pathGradBrush.SetCenterColor(Color(255, 225, 225, 250));
		pathGradBrush.SetFocusScales((REAL)0.0F, 1.0F);
		graphics.FillPath(&pathGradBrush, &path);
			
		Font font(L"Times New Roman", 15);
		SolidBrush sb(Color(255, 0, 0, 0));
		Pen pen(&sb);
		if(_deltaY <= 30) {
			graphics.DrawString(L"Choose A Selection:", 19, &font, PointF(REAL(40 - ps.rcPaint.left), REAL(5 - ps.rcPaint.top)), NULL, &sb);
			graphics.DrawLine(&pen, (40 - ps.rcPaint.left), (30 - ps.rcPaint.top), (210 - ps.rcPaint.left), (30 - ps.rcPaint.top));
		}
		if(_highlightIndex > -1) {
			_bounds[_highlightIndex].X -= ps.rcPaint.left;
			_bounds[_highlightIndex].Y -= (ps.rcPaint.top + 1);
			SolidBrush bHigh(Color(50, 100, 255, 255));
			graphics.FillRectangle(&bHigh, _bounds[_highlightIndex]);
			_bounds[_highlightIndex].X += ps.rcPaint.left;
			_bounds[_highlightIndex].Y += (ps.rcPaint.top + 1);
		}
		StringFormat strForm;
		strForm.SetAlignment(StringAlignmentCenter);
		for(size_t r(((ps.rcPaint.top - 40) / 85)) ; (r < _bounds.size()) && (_bounds[r].Y <= ps.rcPaint.bottom) ; r++) {
			_bounds[r].X -= ps.rcPaint.left;
			_bounds[r].Y -= (ps.rcPaint.top + 1); // Randomly add 1 here to fix a stupid bug with scrolling...
			graphics.DrawRectangle(&pen, _bounds[r]);
			std::stringstream ss;
			ss << _indices.at(r) << ": " << _recordHandler->getRecordTitle(_indices.at(r));
			std::string stitle(ss.str());
			std::wstring title(stitle.begin(), stitle.end());
			RectF rect(REAL(_bounds[r].X - 1), REAL(_bounds[r].Y + 26), REAL(_bounds[r].Width), REAL(_bounds[r].Height));
			graphics.DrawString(title.c_str(), title.size(), &font, rect, &strForm, &sb);
			_bounds[r].X += ps.rcPaint.left;
			_bounds[r].Y += (ps.rcPaint.top + 1);
		}
		
		BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, (ps.rcPaint.right - ps.rcPaint.left), (ps.rcPaint.bottom - ps.rcPaint.top), hdcBuffer, 0, 0, SRCCOPY);
		SetWindowOrgEx(hdc, origin.x, origin.y, NULL);
		DeleteObject(hBitmap);
		DeleteObject(hdcBuffer);
		EndPaint(getHWND(), &ps);
	}
}


void MainMenuPane::mouseMove(LPARAM lParam)
{
	RECT highlightRECT, rcClient;
	getClientDim(&rcClient);
	POINT checkMove;
	GetCursorPos(&checkMove);
	POINT loc = { GET_X_LPARAM(lParam), (GET_Y_LPARAM(lParam) + _deltaY) };
	bool isInBounds((loc.x >= 0) && (loc.x < 275) && (loc.y >= 0) && (loc.y < _height));
	if(isInBounds && ((checkMove.y != _prevMouseLoc.y) || (checkMove.x != _prevMouseLoc.x))) {
		int index((loc.y - 40) / 85);
		bool validNewInd(index != _highlightIndex);
		bool withinX((loc.x > 9) && (loc.x < (_bounds.at(0).Width + 10)));
		bool withinY((loc.y >= _bounds.at(index).Y) && (loc.y <= (_bounds.at(index).Y + _bounds.at(index).Height)));
		if(validNewInd && withinX && withinY) { //If mouse is in bounds of a valid new selection
			_prevHighlightIndex = _highlightIndex;
			_highlightIndex = index;
			_menuFrame->getMenuText()->updateText();
			translateToRECT(_bounds.at(_highlightIndex), highlightRECT, _deltaY);
			HRGN updateRGN = CreateRectRgnIndirect(&highlightRECT);
			if(_prevHighlightIndex > -1) {
				translateToRECT(_bounds.at(_prevHighlightIndex), highlightRECT, _deltaY);
				HRGN updateRGN2 = CreateRectRgnIndirect(&highlightRECT);
				CombineRgn(updateRGN, updateRGN, updateRGN2, RGN_OR);
			}
			redrawWindow(NULL, updateRGN, RDW_INVALIDATE);
		}
		else if((index != _highlightIndex || !withinX || !withinY) && _highlightIndex != -1) { //If mosue moved out of bounds of any selection
			_prevHighlightIndex = _highlightIndex;
			_highlightIndex = -1;
			translateToRECT(_bounds.at(_prevHighlightIndex), highlightRECT, _deltaY);
			redrawWindow(&highlightRECT, NULL, RDW_INVALIDATE);
		}
		_prevMouseLoc.y = 0;
	}
}


void MainMenuPane::mouseWheel(WPARAM wParam, LPARAM lParam)
{
	short scrollVal(GET_WHEEL_DELTA_WPARAM(wParam));
	if(scrollVal > 0) {
		vertScroll(getHWND(), MAKEWPARAM(SB_LINEUP, NULL));
	}
	else {  vertScroll(getHWND(), MAKEWPARAM(SB_LINEDOWN, NULL));  }
}


void MainMenuPane::keyDown(WPARAM wParam)
{
	if((wParam == VK_DOWN)) {
		int newHighightIndex(_highlightIndex + 1);
		if(newHighightIndex < (int)_indices.size()) {
			_prevHighlightIndex = _highlightIndex;
			_highlightIndex = newHighightIndex;
			_menuFrame->getMenuText()->updateText();
			scrollToHighlight();
		}
	}
	else if((wParam == VK_UP)) {
		int newHighlightIndex(_highlightIndex - 1);
		if(newHighlightIndex > -1) {
			_prevHighlightIndex = _highlightIndex;
			_highlightIndex = newHighlightIndex;
			_menuFrame->getMenuText()->updateText();
			scrollToHighlight();
		}
	}
}


void MainMenuPane::charIn(WPARAM wParam)
{
	if (wParam == 13) { //Enter
		_menuFrame->getMenuText()->enterChoice();
	}
	else if (wParam == 27) { //ESC
		_menuFrame->sendMessage(WM_CLOSE, 0, 0);
	}
	else if (wParam == 9) { //TAB
		_menuFrame->getMenuText()->setFocus();
	}
	else if (wParam == 45) { //Hyphen
	_menuFrame->showWindow(SW_MINIMIZE);
	}
	else if (((wParam >= 'A') && (wParam <= 'Z')) || ((wParam >= 'a') && (wParam <= 'z')) || ((wParam >= '0') && (wParam <= '9')) || (wParam == ' ') || (wParam == '_') || (wParam == '-')) {
		_search += (char)wParam;
		int highlightIndexT = _recordHandler->getRecordIndexFor(_search);
		if (highlightIndexT > -1) {
			_prevHighlightIndex = _highlightIndex;
			_highlightIndex = highlightIndexT;
			_menuFrame->getMenuText()->updateText();
			scrollToHighlight();
		}
		else {
			highlightIndexT = _recordHandler->getRecordIndexFor("" + (char)wParam);
			if (highlightIndexT > -1) {
				_prevHighlightIndex = _highlightIndex;
				_highlightIndex = highlightIndexT;
				_search = ("" + (char)wParam);
				_menuFrame->getMenuText()->updateText();
				scrollToHighlight();
			}
			else {  _search = "";  }
		}
	}
}


void MainMenuPane::leftButtonDown(LPARAM lParam)
{
	RECT highlightRECT;
	SetFocus(getHWND());
	POINT loc = { GET_X_LPARAM(lParam) , (GET_Y_LPARAM(lParam) + _deltaY) };
	int index = ((loc.y - 40) / 85);
	if((index < _recordHandler->numOfRecords()) && (loc.x > 9) && (loc.x < (_bounds.at(0).Width + 11))
	&& (loc.y >= _bounds.at(index).Y) && (loc.y <= (_bounds.at(index).Y + _bounds.at(index).Height)))
	{
		_prevHighlightIndex = _highlightIndex;
		_highlightIndex = index;
		_menuFrame->getMenuText()->updateText();
		translateToRECT(_bounds.at(_highlightIndex), highlightRECT, _deltaY);
		HRGN updateRgn = CreateRectRgnIndirect(&highlightRECT);
		if(_prevHighlightIndex > -1) {
			translateToRECT(_bounds.at(_prevHighlightIndex), highlightRECT, _deltaY);
			HRGN updateRgn2 = CreateRectRgnIndirect(&highlightRECT);
			CombineRgn(updateRgn, updateRgn, updateRgn2, RGN_OR);
		}
		redrawWindow(NULL, updateRgn, RDW_INVALIDATE);
		_menuFrame->getMenuText()->enterChoice();
	}
}


void MainMenuPane::create(HWND hwnd)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;
	GetScrollInfo(hwnd, SB_VERT, &si);
	si.fMask  = SIF_PAGE;
	si.nPage  = 200;
	SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
	HFONT hFont(CreateFont(30, 0, 0, 0, FW_SEMIBOLD, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS
	  ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_ROMAN, "Sans Serif"));
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
	DeleteObject(hFont);
}


void MainMenuPane::vertScroll(HWND hwnd, WPARAM wParam)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;
	GetScrollInfo(hwnd, SB_VERT, &si);
	int iVertPos(si.nPos);
	switch(LOWORD(wParam)) {
		case SB_TOP:			si.nPos = si.nMin;			break;
		case SB_BOTTOM:			si.nPos = si.nMax;			break;
		case SB_LINEUP:			si.nPos -= 50;				break;
		case SB_LINEDOWN:		si.nPos += 50;				break;
		case SB_PAGEUP:			si.nPos -= si.nPage;		break;
		case SB_PAGEDOWN:		si.nPos += si.nPage;		break;
		case SB_THUMBTRACK:		si.nPos = si.nTrackPos;		break;
		default:											break;
	}
	si.fMask = SIF_POS;
	SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
	GetScrollInfo(hwnd, SB_VERT, &si);
	if(si.nPos != iVertPos) {
		RECT rcUpdate;
		//int changeY = (si.nPos - iVertPos);
		_deltaY += (si.nPos - iVertPos);
		getClientDim(&rcUpdate);
		/*if(changeY > 0)
			rcUpdate.top = rcUpdate.bottom - changeY;
		else
			rcUpdate.bottom = rcUpdate.top - changeY;*/
		//HRGN updateRgn = CreateRectRgn(rcUpdate.left, (rcUpdate.top + _deltaY), rcUpdate.right, (rcUpdate.bottom + _deltaY));
	    ScrollWindowEx(hwnd, 0, (iVertPos - si.nPos), NULL, NULL, NULL, NULL, SW_INVALIDATE);
		//invalidate(&rcUpdate);
	}
}


LRESULT CALLBACK MainMenuPane::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_MOUSEMOVE:		mouseMove(lParam);			break;
		case WM_MOUSEWHEEL:		mouseWheel(wParam, lParam);	break;
		case WM_KEYDOWN:		keyDown(wParam);			break;
		case WM_CHAR:			charIn(wParam);				break;
		case WM_LBUTTONDOWN:	leftButtonDown(lParam);		break;
		case WM_CREATE:			create(hwnd);				break;
		case WM_VSCROLL:		vertScroll(hwnd, wParam);	break;
		case WM_PAINT:			paint();					break;
		case WM_ERASEBKGND:									return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


void MainMenuPane::scrollToHighlight() {
	RECT highlightRECT, rcClient;
	if(_highlightIndex != -1) {
		GetCursorPos(&_prevMouseLoc);
		getClientDim(&rcClient);
		if(_highlightIndex < 0 || _highlightIndex >= (int)_bounds.size()) {
			std::stringstream ss;
			ss << "_highlightIndex with value " << _highlightIndex << " is outside of the range of the _bounds vector with size " << _bounds.size();
			debugBreak(DEBUG_LOC, ss.str().c_str());
		}
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		GetScrollInfo(getHWND(), SB_VERT, &si);
		si.fMask = SIF_POS;
		HRGN updateRgn;
		int oldDeltaY(_deltaY);
		if(_bounds.at(_highlightIndex).GetBottom() > (_deltaY + rcClient.bottom)) {
			_deltaY = (_bounds.at(_highlightIndex).GetBottom() - rcClient.bottom + 9);
			updateRgn = CreateRectRgn(rcClient.left, (rcClient.top + _deltaY), rcClient.right, (rcClient.bottom + _deltaY));
			ScrollWindowEx(getHWND(), 0, (oldDeltaY - _deltaY), NULL, NULL, updateRgn, NULL, SW_INVALIDATE);
			si.nPos = _deltaY;
			SetScrollInfo(getHWND(), SB_VERT, &si, TRUE);
		}
		else if(_bounds.at(_highlightIndex).GetTop() < _deltaY) {
			_deltaY = (_bounds.at(_highlightIndex).GetTop() - 9);
			updateRgn = CreateRectRgn(rcClient.left, (rcClient.top + _deltaY), rcClient.right, (rcClient.bottom + _deltaY));
			ScrollWindowEx(getHWND(), 0, (oldDeltaY - _deltaY), NULL, NULL, updateRgn, NULL, SW_INVALIDATE);
			si.nPos = _deltaY;
			SetScrollInfo(getHWND(), SB_VERT, &si, TRUE);
		}
		translateToRECT(_bounds.at(_highlightIndex), highlightRECT, _deltaY);
		updateRgn = CreateRectRgnIndirect(&highlightRECT);
		if(_prevHighlightIndex > -1 && _bounds.at(_prevHighlightIndex).Y < (_deltaY + rcClient.bottom) && _bounds.at(_prevHighlightIndex).GetBottom() > _deltaY) {
			translateToRECT(_bounds.at(_prevHighlightIndex), highlightRECT, _deltaY);
			HRGN updateRgn2 = CreateRectRgnIndirect(&highlightRECT);
			CombineRgn(updateRgn, updateRgn, updateRgn2, RGN_OR);
		}
		redrawWindow(NULL, updateRgn, RDW_INVALIDATE);
	}
}


void MainMenuPane::refresh(RecordHandler *recordHandler)
{
	Gdiplus::Rect rect;
	SCROLLINFO si;
	resetHighlight();
	_recordHandler = recordHandler;
	_bounds = std::vector<Gdiplus::Rect>(_recordHandler->numOfRecords() - _recordHandler->numOfFreedRecords());
	_indices = std::vector<int>(_bounds.size());
	for(int r(0), p(0) ; r < _recordHandler->numOfRecords() ; r++) {
		if(!_recordHandler->isRecordCleared(r)) {
			rect = Gdiplus::Rect(10, ((p * 75) + ((p + 1) * 10) + 40), 230, 75);
			_bounds.at(p) = rect;
			_indices.at(p++) = _recordHandler->getRecord(r)->numIndex;
		}
	}
	GetScrollInfo(getHWND(), SB_VERT, &si);
	si.cbSize = sizeof(si);
	si.fMask  = SIF_RANGE;
	si.nMin = 0;
	si.nMax = (((_recordHandler->numOfRecords() - _recordHandler->numOfFreedRecords() - 3) * 75) + ((_recordHandler->numOfRecords() - _recordHandler->numOfFreedRecords()) * 10));
	_height = si.nMax + 265; // 40 is the original offset of first selection, and 225 is the (3 * 75) missing from si.nMax
	if(si.nMax < 101) {
		si.nMax = 101;
	}
	SetScrollInfo(getHWND(), SB_VERT, &si, TRUE);
	update();
}


int MainMenuPane::getHighlightIndex()
{
	if(_highlightIndex == -1) {
		return -1;
	}
	if(_highlightIndex < 0 || _highlightIndex >= (int)_indices.size()) {
		std::stringstream ss;
		ss << "_highlightIndex with value " << _highlightIndex << " is outside of the range of the _indices vector with size " << _indices.size();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	return _indices.at(_highlightIndex);
}


void MainMenuPane::setHighlightIndex(int index)
{
	if(index < 0 || index >= _recordHandler->numOfRecords()) {
		std::stringstream ss;
		ss << "Argument index with value " << index << " is outside of the range of the number of records with capacity " << _recordHandler->numOfRecords();
		debugBreak(DEBUG_LOC, ss.str().c_str());
	}
	_prevHighlightIndex = _highlightIndex;
	if(!_recordHandler->isRecordCleared(index)) {
		if(index >= (int)_indices.size()) {
			_highlightIndex = (_indices.size() - 1);
		}
		else {  _highlightIndex = index;  }
		int diff(std::abs(_indices.at(_highlightIndex) - index));
		while((_indices.at(_highlightIndex) - index) != 0) {
			if(_indices.at(_highlightIndex) > index) {
				_highlightIndex -= diff;
				if(_highlightIndex < 0) {
					_highlightIndex = 0;
				}
			}
			else if(_indices.at(_highlightIndex) < index) {
				_highlightIndex += diff;
				if(_highlightIndex >= (int)_indices.size()) {
					_highlightIndex = (_indices.size() - 1);
				}
			}
			if(diff != 1) {
				diff /= 2;
			}
		}
	}
	else {  _highlightIndex = -1;  }
}


void MainMenuPane::resetHighlight()
{
	_prevHighlightIndex = 0;
	_highlightIndex = 0;
}