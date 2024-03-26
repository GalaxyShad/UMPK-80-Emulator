#pragma once

#include <imgui-SFML.h>
#include <imgui.h>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/WindowStyle.hpp"

#include "components/ui/display/ui-display.hpp"
#include "components/ui/ui-io-register.hpp"
#include "components/ui/ui-keyboard.hpp"

#include "controller.hpp"
#include "gui-app-base.hpp"

class GuiAppCompact : public GuiAppBase {
public:
    GuiAppCompact(const std::string &userProgramFilepath)
        : m_controller(*this) {
        if (userProgramFilepath.substr(
                userProgramFilepath.find_last_of(".") + 1) != "bin") {
            std::cout << "[ERR] \"" << userProgramFilepath << "\" The file path must have a .bin extension.\n";
            return;
        }

        try {
            m_userProgram = m_controller.readBinaryFile(userProgramFilepath);
        } catch (const std::exception& e) {
            std::cout << "[ERR] Failed to read file \"" << userProgramFilepath << "\". Error: " << e.what() << "\n";
            return;
        }

        if (m_userProgram.size() == 0) {
            std::cout << "[ERR] File \"" << userProgramFilepath << "\" is empty.\n";
            return;
        }

        m_window.create(sf::VideoMode(320, 380), "UMPK-80 Emulator",
                        sf::Style::Close);
        ImGui::SFML::Init(m_window);

        m_components.push_back(
            std::make_pair("Display", new UiDisplay(m_controller)));
        m_components.push_back(
            std::make_pair("IO", new UiIoRegister(m_controller)));
        m_components.push_back(
            std::make_pair("Keyboard", new UiKeyboard(m_controller)));

        m_controller.onBtnStart();
    }

    virtual ~GuiAppCompact() {
        for (auto &i : m_components) {
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
    std::vector<std::pair<std::string, IRenderable *>> m_components;

    sf::RenderWindow m_window;
    sf::Clock m_deltaClock;

    std::vector<uint8_t> m_userProgram;

    Controller m_controller;

    void init() {
        // _applyTheme();
    }

    void update() {
        ImGui::SFML::Update(m_window, m_deltaClock.restart());

        auto m_windowSize = sf::Vector2i(m_window.getSize());

        ImGui::SetNextWindowSize(
            ImVec2((float)m_windowSize.x, (float)m_windowSize.y));

        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("##EMU", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoBackground);
        for (auto i : m_components) {
            i.second->render();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        ImGui::End();

        m_window.clear();
        ImGui::SFML::Render(m_window);
        m_window.display();
    }

    void onUmpkOsStartupFinished() override { 
        m_controller.loadProgramToMemory(0x800, m_userProgram);
    }

    void handleEvents() {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(m_window, event);

            if (event.type == sf::Event::Closed) {
                m_window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.control && event.key.code == sf::Keyboard::C) {
                    m_window.close();
                }
            }
        }
    }
};