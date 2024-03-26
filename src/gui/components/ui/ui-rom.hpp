#ifndef UI_ROM_HPP
#define UI_ROM_HPP

#include "../irenderable.hpp"

#include <imgui.h>

#include "../../controller.hpp"
#include "../../vendor/memory_editor/imgui_memory_editor/imgui_memory_editor.h"

class UiRom : public IRenderable {
public:
    UiRom(Controller& controller) : m_controller(controller) {
        m_imGuiMemoryEditor.ReadOnly = true;
    }

    void render() override {
        m_imGuiMemoryEditor.DrawContents((void*)m_controller.getRom(), m_controller.UMPK_ROM_SIZE);
    }
private:
    Controller& m_controller;
    ImGuiMemoryEditor m_imGuiMemoryEditor;
};

class UiRam : public IRenderable {
public:
    UiRam(Controller& controller) : m_controller(controller) {
        m_imGuiMemoryEditor.WriteFn = [](ImU8* data, size_t off, ImU8 d){
            Controller* controller = (Controller*)data;
            controller->setMemory(0x800+off, d);
        };

        m_imGuiMemoryEditor.ReadFn = [](const ImU8* data, size_t off) {
            Controller* controller = (Controller*)data;
            return controller->getRam()[off];
        };
    }

    void render() override {
        m_imGuiMemoryEditor.DrawContents((void*)&m_controller, m_controller.UMPK_ROM_SIZE, 0x800);
    }
private:
    void writeRam(ImU8* data, size_t off, ImU8 d) {
        m_controller.setMemory(off, d);
    }

    Controller& m_controller;
    ImGuiMemoryEditor m_imGuiMemoryEditor;
};

#endif // UI_ROM_HPP
