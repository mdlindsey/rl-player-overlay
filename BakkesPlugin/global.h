#pragma once
#include <iostream>
#include <imgui_stdlib.h> // ImGui::InputText
#include "bakkesmod/plugin/bakkesmodplugin.h"

using namespace std;

namespace Global {
    shared_ptr<GameWrapper> GameWrapper;
    shared_ptr<CVarManagerWrapper> CvarManager;
};
