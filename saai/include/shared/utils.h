#pragma once

#include "CPed.h"
#include "common.h"
#include "ScriptCommands.h"

#include "Log.h"


static void joinGroup(CPed *ped) {
    Log::printInfo("joinGroup");
    if (ped == nullptr || !IsPedPointerValid(ped) || ped == FindPlayerPed()) {
        return;
    }
    Log::printInfo("ped is valid");
    int gr;
    Command<Commands::GET_PLAYER_GROUP>(0, &gr);
    if (!Command<Commands::IS_GROUP_MEMBER>(gr, ped)) {
        Log::printInfo("This Ped is not in player's group, add it");
        Command<Commands::SET_GROUP_LEADER>(gr, FindPlayerPed());
        Log::printInfo("reset leader");
        Command<Commands::SET_GROUP_MEMBER>(gr, ped);
        Log::printInfo("set as follower");
    }
}