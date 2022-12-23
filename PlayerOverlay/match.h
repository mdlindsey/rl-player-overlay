#pragma once
#include <vector>
#include <iostream>
#include "logging.h"
#include "global.h"
#include "player.h"

using namespace std;

class MatchData
{
    private:
    static inline ServerWrapper GAME_STATE = Global::GameWrapper->GetCurrentGameState();
    static bool RefreshGameState()
    {
        MatchData::GAME_STATE = Global::GameWrapper->GetCurrentGameState();
        if (!MatchData::GAME_STATE)
        {
            Log::Error("Game state null");
            MatchData::ResetAll();
            return false;
        }

        return true;
    }

    public:
    inline static int PLAYLIST_ID = 0;
    static void RefreshMeta()
    {
        if (!MatchData::RefreshGameState())
        {
            Log::Warn("Cannot refresh match metadata with null game state");
            MatchData::ResetAll();
            return;
        }

        int playlistId = MatchData::GAME_STATE.GetPlaylist().GetPlaylistId();
        if (playlistId != MatchData::PLAYLIST_ID)
        {
            Log::Info("Playlist updated from \""+ to_string(MatchData::PLAYLIST_ID) + "\" to \"" + to_string(playlistId) + "\"");
            MatchData::PLAYLIST_ID = playlistId;
        }
    }

    // Conversion: 1 uu = 1 cm (e.g. 2778 uu/s = 100 km/h)
    // https://github.com/RLBot/RLBot/wiki/Useful-Game-Values
    inline static float BALL_SPEED_KPH = 0;
    static void RefreshBall()
    {
        if (!MatchData::RefreshGameState())
        {
            Log::Warn("Cannot refresh ball data with null game state");
            return;
        }
        auto ball = MatchData::GAME_STATE.GetBall();
        if (!ball)
        {
            Log::Warn("Cannot refresh ball data with null ball");
            return;
        }
        auto prediction = ball.PredictPosition(0);
        auto velocity = prediction.Velocity;
        float speedInUU = hypot(velocity.X, velocity.Y, velocity.Z);
        float speedInKPH = speedInUU / 27.78;
        if (speedInKPH != MatchData::BALL_SPEED_KPH) {
            MatchData::BALL_SPEED_KPH = speedInKPH;
        }
    }

    inline static float ACTIVE_PLAYER_BOOST_RESERVE = 0;
    static void RefreshBoost()
    {
        if (!MatchData::RefreshGameState())
        {
            Log::Warn("Cannot refresh boost data with null game state");
            return;
        }
        auto car = MatchData::GAME_STATE.GetGameCar();
        if (!car)
        {
            Log::Warn("Cannot refresh boost data with null car");
            return;
        }
        if (car.GetBoostComponent().IsNull())
        {
            MatchData::ACTIVE_PLAYER_BOOST_RESERVE = 0;
        }
        else
        {
            MatchData::ACTIVE_PLAYER_BOOST_RESERVE = car.GetBoostComponent().GetCurrentBoostAmount();
        }
    }

    static inline PriWrapper MY_PRI = Global::GameWrapper->GetPlayerController().GetPRI();
    static inline MatchPlayer * ActivePlayer;
    static bool RefreshActivePlayer()
    {
        auto playerController = Global::GameWrapper->GetPlayerController();
        if (!playerController)
        {
            Log::Error("Player controller null");
            return false;
        }

        MatchData::MY_PRI = playerController.GetPRI();
        MatchData::ActivePlayer = new MatchPlayer(MatchData::MY_PRI, MatchData::MY_PRI, MatchData::PLAYLIST_ID);
        return true;
    }

    inline static vector<MatchPlayer> PLAYERS = {};
    static void RefreshAllPlayers()
    {
        if (!MatchData::RefreshGameState())
        {
            Log::Warn("Cannot refresh player data with null game state");
            return;
        }

        if (!MatchData::RefreshActivePlayer())
        {
            Log::Warn("Cannot refresh player data with null player controller");
            return;
        }

        MatchData::PLAYERS.clear();
        for (auto pri : MatchData::GAME_STATE.GetPRIs())
        {
            if (!pri)
            {
                Log::Warn("Encountered null PRI");
                continue;
            }
            auto Player = new MatchPlayer(pri, MatchData::MY_PRI, MatchData::PLAYLIST_ID);
            MatchData::PLAYERS.push_back(*Player);
        }
        Log::Info("Refreshing all players " + to_string(MatchData::PLAYERS.size())); return;
    }

    static void ResetAll()
    {
        Log::Info("Resetting all match data");
        MatchData::BALL_SPEED_KPH = 0;
        MatchData::ACTIVE_PLAYER_BOOST_RESERVE = 0;
        MatchData::PLAYERS.clear();
    }
};
