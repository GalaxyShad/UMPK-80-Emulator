#pragma once

#include <functional>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <utility>
#include <vector>

#include "components/ui/display/ui-display.hpp"
#include "components/ui/ui-cmd-table.hpp"
#include "components/ui/ui-cpu-control.hpp"
#include "components/ui/ui-decompiler.hpp"
#include "components/ui/ui-io-register.hpp"
#include "components/ui/ui-keyboard.hpp"
#include "components/ui/ui-listing.hpp"
#include "components/ui/ui-program-loader.hpp"
#include "components/ui/ui-rom.hpp"

#include "controller.hpp"

class GuiApp {
public:
    GuiApp() : m_components(), m_controller(*this) {
        m_window.create(sf::VideoMode(1280, 720), "UMPK-80 Emulator");
        ImGui::SFML::Init(m_window);

        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        m_components.push_back(std::make_pair("CMD Table", new UiCmdTable()));
        m_components.push_back(
            std::make_pair("CPU controls", makeCpuControl()));

        m_components.push_back(
            std::make_pair("Display", new UiDisplay(m_controller)));

        UiKeyboardEmits emits;

        emits.emitKeyPress = [](const char *key) {
            std::cout << key << std::endl;
        };

        m_components.push_back(
            std::make_pair("Keyboard", new UiKeyboard(UiKeyboardProps(), emits,
                                                      m_controller)));

        m_components.push_back(
            std::make_pair("Listing", new UiOsListing(m_controller)));

        m_components.push_back(std::make_pair(
            "IO",
            new UiIoRegister(m_controller, UiIoRegisterEmits{[](uint8_t val) {
                                 std::cout << val << std::endl;
                             }})));

        m_components.push_back(std::make_pair("Disassembler", new UiDecompilerWindow(m_controller)));

        m_components.push_back(std::make_pair("Program Loader", new UiProgramLoader(m_controller)));

        m_components.push_back(std::make_pair("ROM", new UiRom(m_controller)));
        m_components.push_back(std::make_pair("RAM", new UiRam(m_controller)));
    }

    virtual ~GuiApp() {
        for (auto& i : m_components) {
            delete i.second;
        }
    }

    void start() {
        init();

        while (m_window.isOpen()) {
            handleEvents();
            update();
        }

        ImGui::SFML::Shutdown(m_window);
    }

private:
    std::vector<UiListingLine> m_listing = std::vector<UiListingLine>();

    std::vector<std::pair<std::string, IRenderable *>> m_components;
    std::vector<uint8_t> m_ram;
    std::vector<uint8_t> m_rom;

    sf::RenderWindow m_window;
    sf::Clock m_deltaClock;

    Controller m_controller;

    void init() {
        // _applyTheme();
    }

    void update() {
        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("DockSpace Demo", NULL, window_flags);
        ImGui::PopStyleVar(2);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        }
        
#ifdef DEBUG
        ImGui::ShowDemoWindow();
        ImGui::ShowMetricsWindow();
#endif // DEBUG

        for (auto i : m_components) {
            ImGui::Begin(i.first.c_str());
            i.second->render();
            ImGui::End();
        }
        
        ImGui::End();

        m_window.clear();
        ImGui::SFML::Render(m_window);
        m_window.display();
    }

    void handleEvents() {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(m_window, event);

            if (event.type == sf::Event::Closed) {
                m_window.close();
            }
        }
    }

    UiCpuControl *makeCpuControl() {
        UiCpuControl *res = nullptr;

        UiCpuControlEmits cpuControlEmits = {};

        cpuControlEmits.onControlButton = [=](const char *btn) {
            std::cout << btn << "\n";
        };

        cpuControlEmits.onFlagChange = [](UiCpuControlFlags flag, bool value) {
            std::cout << flag << " " << value << "\n";
        };

        cpuControlEmits.onRegisterChange = [](UiCpuControlRegister reg,
                                              uint8_t value) {
            std::cout << reg << " " << value << "\n";
        };

        return new UiCpuControl(cpuControlEmits, m_controller);
    }

    void _applyTheme() {
        ImGuiStyle &style = ImGui::GetStyle();

        style.WindowRounding = 8.0;
        style.FramePadding = ImVec2(5, 5);
        style.ItemSpacing = ImVec2(12, 8);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.IndentSpacing = 25.0;
        style.ScrollbarSize = 15.0;
        style.ScrollbarRounding = 15.0;
        style.GrabMinSize = 15.0;
        style.GrabRounding = 7.0;
        style.ChildRounding = 8.0;
        style.FrameRounding = 6.0;

        style.Colors[ImGuiCol_Text] = ImVec4(0.95, 0.96, 0.98, 1.00);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.36, 0.42, 0.47, 1.00);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11, 0.15, 0.17, 1.00);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15, 0.18, 0.22, 1.00);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08, 0.08, 0.08, 0.94);
        style.Colors[ImGuiCol_Border] = ImVec4(0.43, 0.43, 0.50, 0.50);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00, 0.00, 0.00, 0.00);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20, 0.25, 0.29, 1.00);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12, 0.20, 0.28, 1.00);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.09, 0.12, 0.14, 1.00);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09, 0.12, 0.14, 0.65);
        style.Colors[ImGuiCol_TitleBgCollapsed] =
            ImVec4(0.00, 0.00, 0.00, 0.51);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08, 0.10, 0.12, 1.00);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15, 0.18, 0.22, 1.00);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02, 0.02, 0.02, 0.39);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20, 0.25, 0.29, 1.00);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] =
            ImVec4(0.18, 0.22, 0.25, 1.00);
        style.Colors[ImGuiCol_ScrollbarGrabActive] =
            ImVec4(0.09, 0.21, 0.31, 1.00);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.28, 0.56, 1.00, 1.00);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.28, 0.56, 1.00, 1.00);
        style.Colors[ImGuiCol_SliderGrabActive] =
            ImVec4(0.37, 0.61, 1.00, 1.00);
        style.Colors[ImGuiCol_Button] = ImVec4(0.20, 0.25, 0.29, 1.00);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28, 0.56, 1.00, 1.00);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06, 0.53, 0.98, 1.00);
        style.Colors[ImGuiCol_Header] = ImVec4(0.20, 0.25, 0.29, 0.55);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26, 0.59, 0.98, 0.80);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26, 0.59, 0.98, 1.00);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26, 0.59, 0.98, 0.25);
        style.Colors[ImGuiCol_ResizeGripHovered] =
            ImVec4(0.26, 0.59, 0.98, 0.67);
        style.Colors[ImGuiCol_ResizeGripActive] =
            ImVec4(0.06, 0.05, 0.07, 1.00);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61, 0.61, 0.61, 1.00);
        style.Colors[ImGuiCol_PlotLinesHovered] =
            ImVec4(1.00, 0.43, 0.35, 1.00);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90, 0.70, 0.00, 1.00);
        style.Colors[ImGuiCol_PlotHistogramHovered] =
            ImVec4(1.00, 0.60, 0.00, 1.00);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25, 1.00, 0.00, 0.43);
    }
};