#include "pch.h"
#include "cvar.h"
#include "match.h"
#include "plugin.h"
#include "global.h"
#include "logging.h"
#include "overlay.h"
#include "version.h"
#include <imgui_stdlib.h> // ImGui::InputText
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

const char * PluginName = "Player Overlay"; // To change DLL filename use <TargetName> in *.vcxproj
constexpr auto PluginVersion = stringify(V_MAJOR) "." stringify(V_MINOR) "." stringify(V_PATCH) "." stringify(V_BUILD);
BAKKESMOD_PLUGIN(PlayerOverlay, PluginName, PluginVersion, PLUGINTYPE_FREEPLAY);

void PlayerOverlay::onLoad()
{
	Global::GameWrapper = gameWrapper;
	Global::CvarManager = cvarManager;

	Log::SetPrintLevel(Log::Level::Info);
	Log::SetWriteLevel(Log::Level::Info);

	new Cvar("show_name_len", 12, 5, 16);
	new Cvar("show_wins", true);
	new Cvar("show_mvps", true);
	new Cvar("show_goals", true);
	new Cvar("show_assists", true);
	new Cvar("show_saves", true);
	new Cvar("show_mmr_cas", true);
	new Cvar("show_mmr_1v1", true);
	new Cvar("show_mmr_2v2", true);
	new Cvar("show_mmr_3v3", true);
	
	// List of commonly used hooks can be found here:
	// https://wiki.PlayerOverlays.com/functions/commonly_hooked_functions/
	map<string, void (PlayerOverlay::*)(ServerWrapper c, void *p, string e)> ListenerMap =
	{
		{ "Function TAGame.GameEvent_Soccar_TA.Destroyed", &PlayerOverlay::onMatchQuit },
		{ "Function TAGame.PRI_TA.GetScoreboardStats", &PlayerOverlay::onStatRefresh },
	};
	for (auto const& [eventName, eventListener] : ListenerMap)
	{
		gameWrapper->HookEventWithCaller<ServerWrapper>(eventName, bind(
			eventListener, this, placeholders::_1, placeholders::_2, placeholders::_3
		));
	}
}

void PlayerOverlay::onUnload()
{
	
}

// When the plugin is loaded (manually or after build) these settings
// will show in BakkesMod (F2 while in-game) under Plugins > PlayerOverlay
void PlayerOverlay::RenderSettings()
{
	Cvar::Get("show_mmr_cas")->RenderCheckbox("Show Casual MMR");
	Cvar::Get("show_wins")->RenderCheckbox("Show Wins");
	Cvar::Get("show_mvps")->RenderCheckbox("Show MVPs");
	Cvar::Get("show_goals")->RenderCheckbox("Show Goals");
	Cvar::Get("show_assists")->RenderCheckbox("Show Assists");
	Cvar::Get("show_saves")->RenderCheckbox("Show Saves");
	Cvar::Get("show_mmr_1v1")->RenderCheckbox("Show 1v1 MMR");
	Cvar::Get("show_mmr_2v2")->RenderCheckbox("Show 2v2 MMR");
	Cvar::Get("show_mmr_3v3")->RenderCheckbox("Show 3v3 MMR");
	
	ImGui::Spacing();
	Cvar::Get("show_name_len")->RenderSliderInput("Display name max length", 64);
	
	// Padding in case things get close to the bottom and become difficult to read...
	ImGui::NewLine();
	ImGui::NewLine();
	ImGui::NewLine();
}
// More settings rendering boilerplate
string PlayerOverlay::GetPluginName() { return string(PluginName); }
void PlayerOverlay::SetImGuiContext(uintptr_t ctx) { ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx)); }

void PlayerOverlay::onMatchQuit(ServerWrapper caller, void* params, string eventName)
{
	MatchData::PLAYERS.clear();
}

void PlayerOverlay::onStatRefresh(ServerWrapper caller, void* params, string eventName)
{
	MatchData::RefreshMeta();
	MatchData::RefreshBall();
	MatchData::RefreshBoost();
	MatchData::RefreshAllPlayers();
	this->RenderOverlay();
}


map<string, int> TrackerWs = {};
map<string, int> TrackerMVPs = {};
map<string, int> TrackerGoals = {};
map<string, int> TrackerSaves = {};
map<string, int> TrackerAssists = {};
void PlayerOverlay::RenderOverlay()
{
	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		if (!MatchData::PLAYERS.size()) return;

		int overlayWidth = 110;
		if (Cvar::Get("show_mmr_cas") ->toBool()) overlayWidth += 65;
		if (Cvar::Get("show_mmr_1v1")->toBool()) overlayWidth += 65;
		if (Cvar::Get("show_mmr_2v2")->toBool()) overlayWidth += 65;
		if (Cvar::Get("show_mmr_3v3")->toBool()) overlayWidth += 65;
		if (Cvar::Get("show_wins")->toBool()) overlayWidth += 65;
		if (Cvar::Get("show_mvps")->toBool()) overlayWidth += 65;
		if (Cvar::Get("show_goals")->toBool()) overlayWidth += 65;
		if (Cvar::Get("show_assists")->toBool()) overlayWidth += 65;
		if (Cvar::Get("show_saves")->toBool()) overlayWidth += 65;

		Overlay *overlay = new Overlay(canvas);
		overlay->SetWidth(overlayWidth);
		overlay->SetHeight((overlay->lineHeightPx * 2) + (MatchData::PLAYERS.size() * overlay->lineHeightPx));
		overlay->SetColor(0, 0, 0, 200);
		overlay->SetPosition(Overlay::Position::BottomMid);
		overlay->Render();
		overlay->SetColor(255, 255, 255, 255);
		overlay->Print("Name", 120);
		if (Cvar::Get("show_mmr_cas")->toBool()) overlay->Print("Casual", 65);
		if (Cvar::Get("show_mmr_1v1")->toBool()) overlay->Print("1v1", 65);
		if (Cvar::Get("show_mmr_2v2")->toBool()) overlay->Print("2v2", 65);
		if (Cvar::Get("show_mmr_3v3")->toBool()) overlay->Print("3v3", 65);
		if (Cvar::Get("show_wins")->toBool()) overlay->Print("Wins", 65);
		if (Cvar::Get("show_mvps")->toBool()) overlay->Print("MVPs", 65);
		if (Cvar::Get("show_goals")->toBool()) overlay->Print("Goals", 65);
		if (Cvar::Get("show_assists")->toBool()) overlay->Print("Assists", 65);
		if (Cvar::Get("show_saves")->toBool()) overlay->Print("Saves", 65);
		overlay->NextLine();

		int count = 0;
		for(auto p : MatchData::PLAYERS)
		{
			if (p.Platform != "steam" && p.Platform != "epic") continue;
			if (count % 2 == 0)
			{
				overlay->SetColor(125, 255, 255, 255);
			}
			else
			{
				overlay->SetColor(255, 255, 125, 255);
			}
			count++;
			overlay->Print(p.DisplayName.substr(0, Cvar::Get("show_name_len")->toInt()), 120);
			if (Cvar::Get("show_mmr_cas")->toBool()) overlay->Print(to_string(int(p.PlaylistMMR[Global::PlaylistIds::Casual])), 65);
			if (Cvar::Get("show_mmr_1v1")->toBool()) overlay->Print(to_string(int(p.PlaylistMMR[Global::PlaylistIds::Ranked1s])), 65);
			if (Cvar::Get("show_mmr_2v2")->toBool()) overlay->Print(to_string(int(p.PlaylistMMR[Global::PlaylistIds::Ranked2s])), 65);
			if (Cvar::Get("show_mmr_3v3")->toBool()) overlay->Print(to_string(int(p.PlaylistMMR[Global::PlaylistIds::Ranked3s])), 65);
			
			this->LoadTrackerStats(p.Slug);
			if (TrackerWs[p.Slug] != NULL && Cvar::Get("show_wins")->toBool()) overlay->Print(to_string(TrackerWs[p.Slug]), 65);
			if (TrackerMVPs[p.Slug] != NULL && Cvar::Get("show_mvps")->toBool()) overlay->Print(to_string(TrackerMVPs[p.Slug]), 65);
			if (TrackerGoals[p.Slug] != NULL && Cvar::Get("show_goals")->toBool()) overlay->Print(to_string(TrackerGoals[p.Slug]), 65);
			if (TrackerSaves[p.Slug] != NULL && Cvar::Get("show_saves")->toBool()) overlay->Print(to_string(TrackerSaves[p.Slug]), 65);
			if (TrackerAssists[p.Slug] != NULL && Cvar::Get("show_assists")->toBool()) overlay->Print(to_string(TrackerAssists[p.Slug]), 65);
			overlay->NextLine();
		}
	});

}

template <typename F>
void HttpGet(string url, F callback) {
	CurlRequest req; req.url = url;
	HttpWrapper::SendCurlRequest(req, [callback](int code, string result) { callback(result); });
}

map<string, bool> TrackerInit = {};
void PlayerOverlay::LoadTrackerStats(string slug)
{
	if (TrackerInit[slug] == NULL)
	{
		TrackerInit[slug] = true;
		string profileUrl = "https://api.tracker.gg/api/v2/rocket-league/standard/profile/";
		Log::Info("Requesting Profile: " + profileUrl + slug);
		HttpGet(profileUrl + slug, [=](string profileRes) {
			
			if (profileRes.find("mVPs") == std::string::npos)
			{
				Log::Error("Profile Response Failure:");
				Log::Error(profileRes.substr(0, 512));
			}

			json profileJson = json::parse(profileRes); // do mvps
			TrackerWs[slug] = profileJson["data"]["segments"][0]["stats"]["wins"]["value"];
			TrackerMVPs[slug] = profileJson["data"]["segments"][0]["stats"]["mVPs"]["value"];
			TrackerGoals[slug] = profileJson["data"]["segments"][0]["stats"]["goals"]["value"];
			TrackerSaves[slug] = profileJson["data"]["segments"][0]["stats"]["saves"]["value"];
			TrackerAssists[slug] = profileJson["data"]["segments"][0]["stats"]["assists"]["value"];
		});
	}

}
