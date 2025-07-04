#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class FFTwoStick: public BakkesMod::Plugin::BakkesModPlugin
	//,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;
	bool enabled = true;

	//Boilerplate
	void onLoad() override;
	void onUnload() override;
	void Render(CanvasWrapper canvas);
	
	void onTick(std::string eventName);

	// Presses
	bool lastL3R3State = false;
	
	// can forfeit
	bool canForfeit = false;

	// Forfeit
	void ff();
	ControllerInput input;
	int l3Index = -1;
	int r3Index = -1;
public:
	//void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	//void RenderWindow() override; // Uncomment if you want to render your own plugin window
};
