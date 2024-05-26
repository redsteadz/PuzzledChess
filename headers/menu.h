/*******************************************************************************************
*
*   LayoutName v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_LAYOUT_NAME_IMPLEMENTATION
*       #include "gui_layout_name.h"
*
*       INIT: GuiLayoutNameState state = InitGuiLayoutName();
*       DRAW: GuiLayoutName(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 raylib technologies. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"
#include "constant.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string.h>     // Required for: strcpy()

#ifndef GUI_LAYOUT_NAME_H
#define GUI_LAYOUT_NAME_H

typedef struct {
    Vector2 anchor01;
    

    Rectangle layoutRecs[4];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiLayoutNameState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiLayoutNameState InitGuiLayoutName(void);
void GuiLayoutName(GuiLayoutNameState *state);
static void OneShotButton();
static void TimedButton();
static void NoLimitButton();

#ifdef __cplusplus
}
#endif

#endif // GUI_LAYOUT_NAME_H

/***********************************************************************************
*
*   GUI_LAYOUT_NAME IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_LAYOUT_NAME_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiLayoutNameState InitGuiLayoutName(void)
{
    GuiLayoutNameState state = { 0 };

    state.anchor01 = (Vector2){ SCREEN_WIDTH / 2 - 360/2, SCREEN_HEIGHT / 2 - 312/2};
    

    state.layoutRecs[0] = (Rectangle){ state.anchor01.x + 0, state.anchor01.y + 0, 360, 312 };
    state.layoutRecs[1] = (Rectangle){ state.anchor01.x + 96, state.anchor01.y + 72, 160, 48 };
    state.layoutRecs[2] = (Rectangle){ state.anchor01.x + 96, state.anchor01.y + 144, 160, 48 };
    state.layoutRecs[3] = (Rectangle){ state.anchor01.x + 96, state.anchor01.y + 216, 160, 48 };

    // Custom variables initialization

    return state;
}
static void OneShotButton()
{
    // TODO: Implement control logic
}
static void TimedButton()
{
    // TODO: Implement control logic
}
static void NoLimitButton()
{
    // TODO: Implement control logic
}


void GuiLayoutName(GuiLayoutNameState *state)
{
    GuiGroupBox(state->layoutRecs[0], "MainMenu");
    if (GuiButton(state->layoutRecs[1], "One Shot")) OneShotButton(); 
    if (GuiButton(state->layoutRecs[2], "Timed")) TimedButton(); 
    if (GuiButton(state->layoutRecs[3], "No limits")) NoLimitButton(); 
}

#endif // GUI_LAYOUT_NAME_IMPLEMENTATION
