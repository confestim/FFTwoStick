#include "pch.h"
#include "FFTwoStick.h"

BAKKESMOD_PLUGIN(FFTwoStick, "fast forfeit", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void FFTwoStick::onLoad() {
    _globalCvarManager = cvarManager;
    LOG("Plugin loaded!");

    cvarManager->registerCvar("l3r3_forfeit_enabled", "1", "Enable/Disable L3+R3 forfeit shortcut", true, true, 0, true, 1)
        .addOnValueChanged([this](std::string old, CVarWrapper now) {
        enabled = now.getBoolValue();
            });

    enabled = cvarManager->getCvar("l3r3_forfeit_enabled").getBoolValue();

    // Controller inputs
    gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", bind(&FFTwoStick::onTick, this, std::placeholders::_1));

    // Track time to see if ffing is ok
    gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Active.Tick", [this](std::string _) {
        // lambder
        if (!enabled || !gameWrapper->IsInOnlineGame()) return;

        ServerWrapper server = gameWrapper->GetOnlineGame();
        if (server.IsNull()) return;

        auto currentGameTime = server.GetSecondsRemaining();
        canForfeit = (currentGameTime <= 210.0f);
    });

    // Register render
    gameWrapper->RegisterDrawable(std::bind(&FFTwoStick::Render, this, std::placeholders::_1));

    l3Index = gameWrapper->GetFNameIndexByString("XboxTypeS_LeftThumbStick");
    r3Index = gameWrapper->GetFNameIndexByString("XboxTypeS_RightThumbStick");

    LOG("L3+R3 forfeit plugin initialized!");
}

void FFTwoStick::onUnload() {
    LOG("Plugin unloaded!");
}


void FFTwoStick::onTick(std::string _) {
    if (!enabled || !gameWrapper->IsInOnlineGame() || !canForfeit) {
        // Hacky wacky
        lastL3R3State = false;
        canForfeit = false;
        return;
    }


    if (l3Index <= 0 || r3Index <= 0) {
        return;
    }

    // Check if inputs are pressed
    bool l3Pressed = gameWrapper->IsKeyPressed(l3Index);
    bool r3Pressed = gameWrapper->IsKeyPressed(r3Index);

    // Check if both are pressed
    if (l3Pressed && r3Pressed) {
        // Only trigger once per press
        if (!lastL3R3State) {
            lastL3R3State = true;
            LOG("L3+R3 pressed - forfeiting...");
            ff();
        }
    }
    else {
        lastL3R3State = false;
    }
}

void FFTwoStick::ff() {
    auto pri = gameWrapper->GetPlayerController().GetPRI();
    if (!pri.IsNull()) {
        pri.ServerVoteToForfeit();
    }
}

void FFTwoStick::Render(CanvasWrapper canvas) {
    if (!enabled || !canForfeit) return;

    Vector2 screenSize = canvas.GetSize();
    Vector2 position = { 10, screenSize.Y - 30 };

    // Set text color to green
    canvas.SetColor(0, static_cast<unsigned char>(255), 0, static_cast<unsigned char>(255));

    canvas.SetPosition(position);
    canvas.DrawString("Press L3+R3 to FF", 1.0f, 1.0f);
}