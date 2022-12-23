#include "pch.h"
#include "cvar.h"
#include "plugin.h"
#include "global.h"
#include "logging.h"
#include "version.h"
#include <imgui_stdlib.h> // ImGui::InputText

using namespace std;

const char * PluginName = "A1 Plugin Template"; // To change DLL filename use <TargetName> in *.vcxproj
constexpr auto PluginVersion = stringify(V_MAJOR) "." stringify(V_MINOR) "." stringify(V_PATCH) "." stringify(V_BUILD);
BAKKESMOD_PLUGIN(BakkesPlugin, PluginName, PluginVersion, PLUGINTYPE_FREEPLAY);

void BakkesPlugin::onLoad()
{
	Global::GameWrapper = gameWrapper;
	Global::CvarManager = cvarManager;

	Log::SetPrintLevel(Log::Level::Info);
	Log::SetWriteLevel(Log::Level::Info);
	Log::Info("Plugin template loaded");

	new Cvar("hk_magic_num", 420, 69, 420, [=](string name, int newValue, int oldValue) {
		Log::Info("Updated MAGIC NUMBER to " + to_string(newValue));
	});
	
	// List of commonly used hooks can be found here:
	// https://wiki.bakkesplugins.com/functions/commonly_hooked_functions/
	map<string, void (BakkesPlugin::*)(ServerWrapper c, void *p, string e)> ListenerMap =
	{
		{ "Function ProjectX.GameViewportClient_X.EventFocusChanged", &BakkesPlugin::onFocusChange },
	};
	for (auto const& [eventName, eventListener] : ListenerMap)
	{
		gameWrapper->HookEventWithCaller<ServerWrapper>(eventName, bind(
			eventListener, this, placeholders::_1, placeholders::_2, placeholders::_3
		));
	}
}

void BakkesPlugin::onUnload()
{
	Log::Info("Plugin template unloaded");
}

// When the plugin is loaded (manually or after build) these settings
// will show in BakkesMod (F2 while in-game) under Plugins > BakkesPlugin
void BakkesPlugin::RenderSettings()
{
	// Obviously you can delete this spam
	ImGui::Spacing();
	ImGui::Text("########################################################################################################");
	ImGui::Text("##                                                                                                      ");
	ImGui::SameLine();
	ImGui::Text("Dan's BakkesMod Plugin Template");
	ImGui::SameLine();
	ImGui::Text("                                                                                                      ##");
	ImGui::Text("########################################################################################################");
	ImGui::Spacing();
	
	// Rendering settings for cvars is quick and easy...
	Cvar::Get("hk_magic_num")->RenderSliderInput("\nSome Magic Number\n ", 128);
	
	// Padding in case things get close to the bottom and become difficult to read...
	ImGui::NewLine();
	ImGui::NewLine();
	ImGui::NewLine();
}
// More settings rendering boilerplate
string BakkesPlugin::GetPluginName() { return string(PluginName); }
void BakkesPlugin::SetImGuiContext(uintptr_t ctx) { ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx)); }

// Event listeners
void BakkesPlugin::onFocusChange(ServerWrapper caller, void* params, string eventName)
{
	bool focus = *reinterpret_cast<bool*>(params);
	Log::Info("RL window is " + string(focus ? "focused" : "blurred"));
}
