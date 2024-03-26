#pragma once

class GuiAppBase {
public:
    virtual void start() = 0;
    virtual void onUmpkOsStartupFinished() {};
    virtual ~GuiAppBase() {};
};