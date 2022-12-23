#pragma once
#pragma warning(disable:4996)
// Above line prevents compilation error:
// 'PlayerReplicationInfoWrapper::GetUniqueId': Use GetUniqueIdWrapper instead

#include <iostream>
#include "logging.h"
#include "global.h"

using namespace std;

class MatchPlayer
{
    public:

    float MMR = 0;
    int Score = 0;
    int Goals = 0;
    int Shots = 0;
    int Saves = 0;
    int Demos = 0;
    int Assists = 0;
    int OwnGoals = 0;

    map<int, float> PlaylistMMR;

    bool PC = false;
    bool Self = false;
    bool Teammate = false;
    bool PartyLead = false;
    string Slug = "";
    string Platform = "";
    string PlatformUserID = "";
    string DisplayName = "";

    MatchPlayer(PriWrapper pri, PriWrapper myPri, int playlistId)
    {
        // Set display name
        this->DisplayName = pri.GetPlayerName().ToString();

        // Derive platform and platform-specific UID
        switch(pri.GetUniqueIdWrapper().GetPlatform())
        {
            case OnlinePlatform_Steam:
                this->PC = true;
                this->Platform = "steam";
                this->PlatformUserID = to_string(pri.GetUniqueId().ID);
                break;
            case OnlinePlatform_Epic:
                this->PC = true;
                this->Platform = "epic";
                this->PlatformUserID = this->DisplayName;
                break;
            default:
                this->PC = false;
                this->Platform = "console";
                this->PlatformUserID = this->DisplayName;
        }

        this->Slug = this->Platform + "/" + this->PlatformUserID;
        
        // Assign static attributes
        this->Score = pri.GetMatchScore();
        this->Goals = pri.GetMatchGoals();
        this->Shots = pri.GetMatchShots();
        this->Saves = pri.GetMatchSaves();
        this->Demos = pri.GetMatchDemolishes();
        this->Assists = pri.GetMatchAssists();
        this->OwnGoals = pri.GetMatchOwnGoals();
        
        // Assign dynamic attributes
        auto playerId = pri.GetUniqueIdWrapper();
        auto mmrWrapper = Global::GameWrapper->GetMMRWrapper();
        this->MMR = mmrWrapper.GetPlayerMMR(playerId, playlistId);
        this->PlaylistMMR = {};
        this->PlaylistMMR.emplace(Global::PlaylistIds::Casual, mmrWrapper.GetPlayerMMR(playerId, Global::PlaylistIds::Casual));
        this->PlaylistMMR.emplace(Global::PlaylistIds::Ranked1s, mmrWrapper.GetPlayerMMR(playerId, Global::PlaylistIds::Ranked1s));
        this->PlaylistMMR.emplace(Global::PlaylistIds::Ranked2s, mmrWrapper.GetPlayerMMR(playerId, Global::PlaylistIds::Ranked2s));
        this->PlaylistMMR.emplace(Global::PlaylistIds::Ranked3s, mmrWrapper.GetPlayerMMR(playerId, Global::PlaylistIds::Ranked3s));
        this->PlaylistMMR.emplace(Global::PlaylistIds::Hoops, mmrWrapper.GetPlayerMMR(playerId, Global::PlaylistIds::Hoops));
        this->PlaylistMMR.emplace(Global::PlaylistIds::Rumble, mmrWrapper.GetPlayerMMR(playerId, Global::PlaylistIds::Rumble));
        this->PlaylistMMR.emplace(Global::PlaylistIds::Dropshot, mmrWrapper.GetPlayerMMR(playerId, Global::PlaylistIds::Dropshot));
        this->PlaylistMMR.emplace(Global::PlaylistIds::Snowday, mmrWrapper.GetPlayerMMR(playerId, Global::PlaylistIds::Snowday));
        
        // Check if they are teammate (blue = 0, orange = 1, spectate = 255)
        this->Teammate = pri.GetTeamNum() == myPri.GetTeamNum();
        // Check if they are party lead
        this->PartyLead = pri.GetPartyLeaderID().GetUID() == playerId.GetUID();
        // Check if this player is the active plugin user
        this->Self = playerId.GetUID() == myPri.GetUniqueIdWrapper().GetUID();
    }
};