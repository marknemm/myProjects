#ifndef ACTION_MACROS_H
#define ACTION_MACROS_H


// Bit Masks
#define DURATION			16
#define FIRST_POINT			32
#define SCROLL				64
#define KEY					128
#define MOUSE				256
#define SECOND_POINT		512
#define RELATIV				1024
#define EXTRA_STAGE			2048


// Specific Actions
#define L_CLICK				289
#define L_PRESS				2352
#define R_CLICK				290
#define D_CLICK				291
#define SCR_UP				2400
#define SCR_DOWN			2401
#define CLICK_N_DRAG		800
#define TEXT_INP			2048
#define K_TYPE				2177 //129
#define K_PRESS				2192
#define REL_MOUSE_ACT		3872
#define NO_ACT				288
#define DEFAULT_ACT			0


int indexToAction(int index);
bool isActionValid(int action);


#endif