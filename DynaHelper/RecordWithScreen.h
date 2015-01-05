#ifndef  RECORD_WITH_SCREEN_H
#define  RECORD_WITH_SCREEN_H
#include <Windows.h>
#include <sstream>
#include <string>
#include <windowsx.h>
#include <gdiplus.h>
#include <vector>
#include <boost/thread/thread.hpp>
#include <time.h>
#include "ActionMacros.h"
#include "Point.h"
#include "MorphWind.h"
#include "RecordScreen.h"
#include "KeyInputVerificationMenu.h"
#include "NumOfScrollsMenu.h"
#include "PressDurationMenu.h"
#include "TextInput.h"
#include "RecordHandler.h"
#include "Main.h"
#include "ActionMenu.h"
#include "ActionDelayMenu.h"
#include "ActionOffsetMenu.h"
#include "FinishedMenu.h"
#include "TitleMenu.h"
#include "OffsetStruct.h"


#define  ACTION_SLEEP_AMT 400


class RecordWithScreen {

	RecordStruct _recordStruct;
	RecordScreen *_recordScreen;
	int _recordsAdded;
	int _index;


public:


	RecordWithScreen();


private:


	void activate(int stage);
	void continueRecording(int keyCode, int stage);
	bool stage1Action(int keyCode, int stage, int action, int &duration, OffsetStruct &offset, std::string &text);
	bool enableActionMenu(ActionMenu **am, int &action);
	void setRecordStructAction(int action, int stage);
	bool enableTextInputMenu(std::string &text);
	bool enableKeyInputVerificationMenu(int keyCode);
	bool enableNumScrollsMenu(int &duration);
	bool enableOffsetActionMenu(OffsetStruct &offset, int &duration);
	bool enablePressDurationMenu(int &duration, int stage);
	int getRecordScreenResults();
	void fillData(int stage, std::string &text, OffsetStruct &offset, int &duration, int &keyCode);
	bool enableActionDelayMenu(int keyCode);
	bool enableFinishedMenu(bool &finished, int keyCode);
	bool enableTitleMenu(int keyCode);
	void save();
	void reset();
};


#endif