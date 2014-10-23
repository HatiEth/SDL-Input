#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <map>
#include <memory>

#include <SDL2/SDL_events.h>

#include "sdli_definitions.h"

namespace sdli {

class Processor
{
    std::map<sdli::ContextId, std::unique_ptr<sdli::InputContext>> contextMap;

    std::unique_ptr<sdli::InputDevice> keyboard;

public:
    Processor();

    sdli::InputContext* createContext(const sdli::ContextId& contextId);
    sdli::InputDevice* getDevice(sdli::InputType type);



    void handleSdlEvents(const SDL_Event& e);



};

} // sdli

#endif // PROCESSOR_H