#include "Settings.h"

#include "utils/INISettingCollection.h"

namespace settings
{
	using namespace utils;

	void Init(const std::string& a_iniFileName)
	{
		INISettingCollection* iniSettingCollection = INISettingCollection::GetSingleton();

		{
			using namespace debug;
			iniSettingCollection->AddSettings(
				MakeSetting("uLogLevel:Debug", static_cast<std::uint32_t>(logLevel)));
		}

		{
			using namespace display;
			iniSettingCollection->AddSettings(
				MakeSetting("fPositionX:Display", positionX),
				MakeSetting("fPositionY:Display", positionY),
				MakeSetting("fScale:Display", scale),
				MakeSetting("fOpacity:Display", opacity),
				MakeSetting("uShape:Display", shape),
				MakeSetting("bShowOnGameStart:Display", showOnGameStart),
				MakeSetting("sControlHideTip:Display", controlHideTip),
				MakeSetting("sControlMoveTip:Display", controlMoveTip),
				MakeSetting("sControlZoomTip:Display", controlZoomTip)
			);
		}

		{
			using namespace controls;
			iniSettingCollection->AddSettings(
				MakeSetting("bFollowPlayerCameraRotation:Controls", followPlayerCameraRotation),
				MakeSetting("fHoldDownToControlSecs:Controls", holdDownToControlSecs),
				MakeSetting("fDelayToHideControlsSecs:Controls", delayToHideControlsSecs)
			);
		}

		if (!iniSettingCollection->ReadFromFile(a_iniFileName))
		{
			logger::warn("Could not read {}, falling back to default options", a_iniFileName);
		}

		{
			using namespace debug;
			logLevel = static_cast<logger::level>(iniSettingCollection->GetSetting<std::uint32_t>("uLogLevel:Debug"));
		}

		{
			using namespace display;
			positionX = iniSettingCollection->GetSetting<float>("fPositionX:Display");
			positionY = iniSettingCollection->GetSetting<float>("fPositionY:Display");
			scale = iniSettingCollection->GetSetting<float>("fScale:Display");
			opacity = iniSettingCollection->GetSetting<float>("fOpacity:Display");
			shape = iniSettingCollection->GetSetting<std::uint32_t>("uShape:Display");
			showOnGameStart = iniSettingCollection->GetSetting<bool>("bShowOnGameStart:Display");
			controlHideTip = iniSettingCollection->GetSetting<const char*>("sControlHideTip:Display");
			controlMoveTip = iniSettingCollection->GetSetting<const char*>("sControlMoveTip:Display");
			controlZoomTip = iniSettingCollection->GetSetting<const char*>("sControlZoomTip:Display");
		}

		{
			using namespace controls;
			followPlayerCameraRotation = iniSettingCollection->GetSetting<bool>("bFollowPlayerCameraRotation:Controls");
			holdDownToControlSecs = iniSettingCollection->GetSetting<float>("fHoldDownToControlSecs:Controls");
			delayToHideControlsSecs = iniSettingCollection->GetSetting<float>("fDelayToHideControlsSecs:Controls");
		}
	}
}