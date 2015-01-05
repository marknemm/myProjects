#ifndef  DATA_OPTIONS_H
#define  DATA_OPTIONS_H
#include <Windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <Gdiplusenums.h>
#include <Gdiplusgraphics.h>
#include <string>
#include <vector>
#include "Main.h"
#include "ActionMacros.h"
#include "MorphWind.h"
#include "RecordHandler.h"
#include "RecordScreen.h"
//#include "CustDropWindow.h"


#define CMD_EXIT_AND_SAVE 1
#define CMD_EXIT		  2

#define INDEX_HEADER 1
#define ALPHA_HEADER 2
#define TIME_HEADER  3

#define ORDER_BY_INDEX 1
#define ORDER_BY_ALPHA 2
#define ORDER_BY_TIME  3

#define ARROW_NORMAL 1
#define ARROW_HOVER  2
#define ARROW_PRESS  3

#define LOC_1 1
#define LOC_2 2

#define SET_TITLE_BUTTON_ID				0
#define SET_DELAY_BUTTON_ID				1
#define SET_LOCATION_BUTTON_ID			2
#define SET_KEY_BUTTON_ID				3
#define SET_DURATION_BUTTON_ID			4
#define SET_TEXT_BUTTON_ID				5
#define SET_SCROLLS_BUTTON_ID			6
#define DUPLICATE_RECORD_BUTTON_ID		7
#define ADD_RECORD_BUTTON_ID			8
#define ADD_DIRECTIVE_BUTTON_ID			9
#define SWAP_RECORDS_BUTTON_ID			10
#define DELETE_RECORD_BUTTON_ID			11
#define DELETE_DIRECTIVE_BUTTON_ID		12
#define DELETE_ALl_RECORDS_BUTTON_ID	13
#define SAVE_AND_EXIT_BUTTON_ID			14
#define EXIT_BUTTON_ID					15


class DataOptions : public MorphWind {

	class DataScroll : public MorphWind {
		public:
			POINT _prevMouseLoc;
			int _highlightInd;
			int _maxDirective;
			std::map<int, int> _selectedInd;
			std::list<Record*> _selectedRecords;
			bool _ctrlDown;
			bool _leftPress;
			DataScroll(DataOptions *dataOptions);
			HWND getHWND() const;
			std::list<Record*>* getSelectedRecords();
			int getHighlightInd() const;
			void orderByIndex();
			void orderByAlpha();
			void orderByTime();
			void adjustScrollInfo();
			void enableSwapMode(bool enable);
			bool isSwapModeEnabled() const;
			void selectAndScrollToRecords(std::list<Record*> &records);
		private:
			DataOptions *_dataOptions;
			int _order;
			int _deltaY;
			bool _swapModeEnabled;
			int getNumDirectivesOfHighlight();
			int getRecordIndex(int dataScrollInd);
			void selectIndividualIndex();
			void scrollToHighlight();
			void create();
			void paintRecords(Gdiplus::Graphics &graphics, RECT &rcClip, Gdiplus::Font &font);
			void paint();
			void vScroll(WPARAM wParam);
			void mouseMove(LPARAM lParam);
			void leftButtonDown();
			void leftButtonUp();
			void mouseWheel(WPARAM wParam, LPARAM lParam);
			void keyDown(WPARAM wParam);
			void keyUp(WPARAM wParam);
			LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
	};

	class DirectiveWind : public MorphWind {
			DataOptions *_dataOptions;
			int _directive;
		public:
			DirectiveWind();
			void initialize(DataOptions *parent);
			int getDirective() const;
			void setDirective(int directive);
	};

	class TitleEdit : public MorphWind {
			std::string _title;
			WNDPROC _origEditProc;
		public:
			TitleEdit();
			void initialize(MorphWind *parent);
			bool isInputValid();
			std::string& getTitle();
		private:
			LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
	};

	class DelayEdit : public MorphWind {
			double _delay;
			bool _lowerDelayOk;
			WNDPROC _origEditProc;
			int _buttonID;
		public:
			DelayEdit();
			void initialize(MorphWind *parent, bool isDuration);
			bool isInputValid();
			int getDelay();
		private:
			LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
	};

	class LocationEdit : public MorphWind {
		public:
			LocationEdit();
			void initialize(MorphWind *parent, int loc);
	};

	class KeyEdit : public MorphWind {
		public:
			KeyEdit();
			void initialize(MorphWind *parent);
			void setText(int keyCode);
	};

	class TextEdit : public MorphWind {
			std::string _input;
			WNDPROC _origEditProc;
		public:
			TextEdit();
			void initialize(MorphWind *parent);
			bool isTextValid();
			std::string& getText();
		private:
			LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
	};

	class ScrollEdit : public MorphWind {
			int _numScrolls;
			WNDPROC _origEditProc;
		public:
			ScrollEdit();
			void initialize(MorphWind *parent);
			bool isInputValid();
			int getNumScrolls() const;
		private:
			LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
	};

	friend DataScroll;

	RECT _updateRgnHeader;
	RECT _updateRgnUpArrow;
	RECT _updateRgnDownArrow;
	DirectiveWind _directiveWind;
	DataScroll _dataScroll;
	bool _changeMade;
	int _exitCmd;
	int _highlightHeader;
	int _upState;
	int _downState;
	RecordScreen *_recordScreen;
	MorphWind _buttons[16];
	MorphWind _actionDropWind;
	MorphWind _relativeActionDropWind;
	TitleEdit _titleEdit;
	DelayEdit _delayEdit;
	DelayEdit _durationEdit;
	LocationEdit _locationEdit[2];
	TextEdit _textEdit;
	KeyEdit _keyEdit;
	ScrollEdit _scrollEdit;


public:


	DataOptions();
	~DataOptions();
	void initialize(MorphWind *parent);
	void setFieldsEnabled(bool enable, int action);
	void selectRecord();
	void selectDirective(int directive);
	std::list<Record*>* getSelectedRecords();
	int getDirective() const;
	int exitCmd() const;
	bool wasChangeMade() const;


private:


	int getSelectedAction();
	int getSelectedRelativeAction();
	void getSelectedTitle(std::string &title);
	void getSelectedLocation(Point &loc);
	void getSelectedLocation2(Point &loc2);
	void getSelectedDelay(double &delay);
	void getSelectedDuration(double &duration);
	void getSelectedText(std::string &text);
	int getSelectedKey();
	void getStringOfDouble(std::stringstream &ss, double &doub);
	void createButtons();
	void createEdits();
	void initActionDrop();
	void initRelativeActionDrop();
	void drawScrollHeader(Gdiplus::Graphics &graphics, RECT &rcClip);
	void drawDirective(Gdiplus::Graphics &graphics, RECT &rcClip, Gdiplus::StringFormat &stringform);
	void paint();
	void mouseMove(LPARAM lParam);
	void leftButtonDown();
	void leftButtonUp();
	void mouseWheel(WPARAM wParam, LPARAM lParam);
	void command(WPARAM wParam, LPARAM lParam);
	void keyDown(WPARAM wParam);
	void appMsg(UINT message);
	LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
};


#endif