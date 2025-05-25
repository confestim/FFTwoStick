#include "pch.h"
#include "FFTwoStick.h"

BAKKESMOD_PLUGIN(FFTwoStick, "fast forfeit", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void FFTwoStick::onLoad() {
    _globalCvarManager = cvarManager;
    LOG("Plugin loaded!");

    // Register CVars for configuration
    cvarManager->registerCvar("l3r3_forfeit_enabled", "1", "Enable/Disable L3+R3 forfeit shortcut", true, true, 0, true, 1)
        .addOnValueChanged([this](std::string old, CVarWrapper now) {
        enabled = now.getBoolValue();
            });

    // Initial setup
    enabled = cvarManager->getCvar("l3r3_forfeit_enabled").getBoolValue();

    // Hook the game tick event to check for controller inputs
    // https://github.com/haltepunkt/ControllerOverlay/blob/master/ControllerOverlay/ControllerOverlay.cpp
    gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", bind(&FFTwoStick::onTick, this, std::placeholders::_1));

    // Hook into car (lags a lot)
    /* gameWrapper->HookEventWithCaller<CarWrapper>(
        "Function TAGame.Car_TA.SetVehicleInput",
        [this](CarWrapper car, void* params, std::string eventName) {
            OnTick(car, (ControllerInput*)params);
        }
    );*/

    // For debugging
    gameWrapper->RegisterDrawable(std::bind(&FFTwoStick::Render, this, std::placeholders::_1));

    LOG("L3+R3 forfeit plugin initialized!");
}

void FFTwoStick::onUnload() {
    LOG("Plugin unloaded!");
}

void FFTwoStick::onTick(std::string eventName)
{
    if (!enabled) {
        return;
    }

    // Get the input indices
    int l3Index = gameWrapper->GetFNameIndexByString("XboxTypeS_LeftThumbStick");
    int r3Index = gameWrapper->GetFNameIndexByString("XboxTypeS_RightThumbStick");
    if (l3Index <= 0 || r3Index <= 0) {
        return; // Invalid input indices
    }

    // Check if inputs are pressed
    bool l3Pressed = gameWrapper->IsKeyPressed(l3Index);
    bool r3Pressed = gameWrapper->IsKeyPressed(r3Index);

    // Update the debug display
    debugL3Pressed = l3Pressed;
    debugR3Pressed = r3Pressed;

    // Check if both are pressed
    if (l3Pressed && r3Pressed) {
        // Only trigger once per press
        if (!lastL3R3State) {
            lastL3R3State = true;
            LOG("L3+R3 pressed - would execute forfeit");
        }
    }
    else {
        lastL3R3State = false;
    }
}

//void FFTwoStick::ff() {
//
//}

void FFTwoStick::Render(CanvasWrapper canvas) {
    if (!enabled) return;

    Vector2 screenSize = canvas.GetSize();
    Vector2 position = { 10, screenSize.Y - 50 };

    // Set text color to white
    canvas.SetColor(255, 255, 255, 255);

    // Display L3/R3 state
    std::string status = "L3: " + std::string(debugL3Pressed ? "PRESSED" : "released") +
        " | R3: " + std::string(debugR3Pressed ? "PRESSED" : "released");

    canvas.SetPosition(position);
    canvas.DrawString(status, 2.0f, 2.0f);

    // Add plugin status
    position.Y += 20;
    canvas.SetPosition(position);
    canvas.DrawString(std::string("FFTwoStick: ") + (enabled ? "ENABLED" : "disabled"), 2.0f, 2.0f);
}