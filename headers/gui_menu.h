/*******************************************************************************************
*
*   Menu v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_MENU_IMPLEMENTATION
*       #include "gui_menu.h"
*
*       INIT: GuiMenuState state = InitGuiMenu();
*       DRAW: GuiMenu(&state);
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

#ifndef GUI_MENU_H
#define GUI_MENU_H

typedef struct {
    Vector2 anchor01;
    

    Rectangle layoutRecs[4];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiMenuState;

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
GuiMenuState InitGuiMenu(void);
void GuiMenu(GuiMenuState *state, GAME_STATE &gameState);
static void OneShotButton(GAME_STATE &gameState);
static void TimedButton(GAME_STATE &gameState);
static void NoLimitButton(GAME_STATE &gameState);

#ifdef __cplusplus
}
#endif

#endif // GUI_MENU_H

/***********************************************************************************
*
*   GUI_MENU IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_MENU_IMPLEMENTATION)

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
GuiMenuState InitGuiMenu(void)
{
    GuiMenuState state = { 0 };

    state.anchor01 = (Vector2{SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 156});
    

    state.layoutRecs[0] = (Rectangle){ state.anchor01.x + 0, state.anchor01.y + 0, 360, 312 };
    state.layoutRecs[1] = (Rectangle){ state.anchor01.x + 96, state.anchor01.y + 72, 160, 48 };
    state.layoutRecs[2] = (Rectangle){ state.anchor01.x + 96, state.anchor01.y + 144, 160, 48 };
    state.layoutRecs[3] = (Rectangle){ state.anchor01.x + 96, state.anchor01.y + 216, 160, 48 };

    // Custom variables initialization

    return state;
}
static void OneShotButton(GAME_STATE &gameState)
{
    // TODO: Implement control logic
    gameState = GAME_STATE::ONE_SHOT;
}
static void TimedButton(GAME_STATE &gameState)
{
    // TODO: Implement control logic
    gameState = GAME_STATE::TIMED;
}
static void NoLimitButton(GAME_STATE &gameState)
{
    // TODO: Implement control logic
    gameState = GAME_STATE::NO_LIMIT;
}


void GuiMenu(GuiMenuState *state, GAME_STATE &gameState)
{
    GuiGroupBox(state->layoutRecs[0], "MainMenu");
    if (GuiButton(state->layoutRecs[1], "One Shot")) OneShotButton(gameState); 
    if (GuiButton(state->layoutRecs[2], "Timed")) TimedButton(gameState); 
    if (GuiButton(state->layoutRecs[3], "No limits")) NoLimitButton(gameState); 
}

#endif // GUI_MENU_IMPLEMENTATION
