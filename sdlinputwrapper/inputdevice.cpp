#include "inputdevice.h"

namespace sdli {

namespace {
    const bool IS_RELEASED_UNDEFINED = true;
    const bool IS_PRESSED_UNDEFINED = false;
}

InputDevice::InputDevice()
{
}

void InputDevice::poll()
{
    auto sdl_keystate = SDL_GetKeyboardState(NULL);
    for(auto& i : keyboardKeys)
    {
        auto state = sdl_keystate[i.first];
        logicDigitalData[i.second].previousStatus = logicDigitalData[i.second].currentStatus;
        logicDigitalData[i.second].currentStatus = state;
    }


}

void InputDevice::push(InputType type, int rawInput, int value)
{
    perFrameCaptures.emplace_back(RawInputData{type, rawInput, value});
}

//TODO: evaluate isPressed correctly - detect changes over multiple samples
void InputDevice::dispatch()
{
    for(auto& i : logicDigitalData)
    {
        i.second.previousStatus = i.second.currentStatus;
        i.second.currentStatus = false;
    }

    for(auto& d : perFrameCaptures)
    {
        switch(d.type)
        {
        case InputType::Keyboard:
            logicDigitalData[keyboardKeys[static_cast<SDL_Scancode>(d.rawInput)]].currentStatus |= d.pollResult;
            break;

        case InputType::GamecontrollerButton:
            logicDigitalData[gameControllerButtons[static_cast<SDL_GameControllerButton>(d.rawInput)]].currentStatus |= d.pollResult;
            break;

        case InputType::GamecontrollerAxis:
            logicAnalogData[gameControllerAxes[static_cast<SDL_GameControllerAxis>(d.rawInput)].axis].currentStatus = static_cast<float>(d.pollResult);
            // / gameControllerAxes[static_cast<SDL_GameControllerAxis>(d.rawInput)].normalizeValue;
            break;
//        case InputType::MouseButton:
//            // TBA - Req for public ?
//            break;
//        case InputType::MouseAxis:
//            // TBA - Req for public ?
//            break;
        }
    }

    perFrameCaptures.clear();
}

float InputDevice::getRange(InputAxis axis)
{
    if(logicAnalogData.find(axis) == logicAnalogData.end())
        return 0.0f;

    return logicAnalogData[axis].currentStatus;
}

bool InputDevice::isPressed(InputAction action)
{
    if(logicDigitalData.find(action) == logicDigitalData.end())
    {
        return IS_PRESSED_UNDEFINED;
    }

    return logicDigitalData[action].currentStatus
            && logicDigitalData[action].previousStatus != logicDigitalData[action].currentStatus;
}

bool InputDevice::isReleased(InputAction action)
{
    if(logicDigitalData.find(action) == logicDigitalData.end())
    {
        return IS_RELEASED_UNDEFINED;
    }

    return !logicDigitalData[action].currentStatus
            && logicDigitalData[action].previousStatus != logicDigitalData[action].currentStatus;;
}

bool InputDevice::isDown(InputAction action)
{
    if(logicDigitalData.find(action) == logicDigitalData.end())
    {
        return IS_PRESSED_UNDEFINED;
    }

    return logicDigitalData[action].currentStatus;
}

bool InputDevice::isUp(InputAction action)
{
    if(logicDigitalData.find(action) == logicDigitalData.end())
    {
        return IS_RELEASED_UNDEFINED;
    }

    return logicDigitalData[action].currentStatus;
}

void InputDevice::mapDigital(SDL_Scancode raw, InputAction a)
{
    keyboardKeys.emplace(std::make_pair(raw, a));
}

void InputDevice::mapDigital(SDL_GameControllerButton raw, InputAction a)
{
    gameControllerButtons.emplace(std::make_pair(raw, a));
}

void InputDevice::mapRange(SDL_GameControllerAxis raw, InputAxis a, float normalize)
{
    gameControllerAxes.emplace(std::make_pair(raw, InputAxisMapping{a, normalize}));
}

}
