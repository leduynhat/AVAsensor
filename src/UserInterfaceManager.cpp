#include "UserInterfaceManager.h"
#define _SERIAL_DEBUG_
UserInterfaceManager::UserInterfaceManager(/* args */)
{
}

UserInterfaceManager::~UserInterfaceManager()
{
}
void UserInterfaceManager::init()
{
    this->display.init();
    this->led_signal.init();
    this->user_input.init();
    #ifdef _SERIAL_DEBUG_UI_MANAGER_
        Serial.println("->User interface init successfully");
    #endif
}
void UserInterfaceManager::show(SCREEN_ID _screen)
{
    this->display.setDisplayScreen(_screen);
    this->display.showScreen();
}