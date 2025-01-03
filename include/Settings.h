#pragma once

namespace SKSE::log
{
	using level = spdlog::level::level_enum;
}
namespace logger = SKSE::log;

namespace settings
{
	void Init(const std::string& a_iniFileName);

	// Default values

	namespace debug
	{
		inline logger::level logLevel = logger::level::trace;
	}

	namespace display
	{
		inline float positionX = 0.65F;
		inline float positionY = 0.2F;
		inline float scale = 1.0F;
		inline float opacity = 1.0F;
		inline std::uint32_t shape = 0;
		inline bool showOnGameStart = true;
		inline const char* controlHideTip = "Hold to control/tap to hide";
		inline const char* controlMoveTip = "Move";
		inline const char* controlZoomTip = "Zoom";
	}

	namespace controls
	{
		inline bool followPlayerCameraRotation = true;
		inline float holdDownToControlSecs = 0.15F;
		inline float delayToHideControlsSecs = 1.0F;
	}
}