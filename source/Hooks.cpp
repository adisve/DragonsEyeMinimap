#include "Hooks.h"

#include "utils/Logger.h"

#include "RE/H/HUDMenu.h"

#include "RE/RTTI.h"

#include "MiniMap.h"

#include "RE/R/RenderPassCache.h"

void AcceptHUDMenu(RE::HUDMenu* a_hudMenu, RE::FxDelegateHandler::CallbackProcessor* a_gameDelegate)
{
	hooks::HUDMenu::Accept(a_hudMenu, a_gameDelegate);

	a_gameDelegate->Process("SetLocalMapExtents", MiniMap::SetLocalMapExtents);
}

void AdvanceMovieHUDMenu(RE::HUDMenu* a_hudMenu, float a_interval, std::uint32_t a_currentTime)
{
	hooks::HUDMenu::AdvanceMovie(a_hudMenu, a_interval, a_currentTime);

	if (auto miniMap = MiniMap::GetSingleton())
	{
		RE::LocalMapMenu* localMap = miniMap->GetLocalMap();
		
		if (!localMap)
		{
			miniMap->InitLocalMap();
			if (localMap = miniMap->GetLocalMap())
			{
				a_hudMenu->menuFlags.set(RE::UI_MENU_FLAGS::kRendersOffscreenTargets);
			}
		}

		if (localMap)
		{
			RE::NiPoint3 playerPos = RE::PlayerCharacter::GetSingleton()->GetPosition();

			RE::LocalMapCamera* localMapCamera = localMap->localCullingProcess.GetLocalMapCamera();

			localMapCamera->defaultState->initialPosition.x = playerPos.x;
			localMapCamera->defaultState->initialPosition.y = playerPos.y;
			localMapCamera->defaultState->translation = { 0, 0, 0 };

			localMap->Advance();
		}
	}
}

void PreDisplayHUDMenu(RE::HUDMenu* a_hudMenu)
{
	if (auto miniMap = MiniMap::GetSingleton())
	{
		if (!RE::UI::GetSingleton()->IsMenuOpen(RE::MapMenu::MENU_NAME))
		{
			if (RE::LocalMapMenu* localMap = miniMap->GetLocalMap())
			{
				using namespace std::chrono;

				static std::chrono::time_point<system_clock> last;

				std::chrono::time_point<system_clock> now = system_clock::now();

				auto diff = duration_cast<milliseconds>(now - last);

				if (diff > 100ms)
				{
					RE::NiPointer<RE::NiNode> fogOfWarOverlay = localMap->localCullingProcess.GetFogOfWarOverlay();

					if (fogOfWarOverlay)
					{
						RE::NiTObjectArray<RE::NiPointer<RE::NiAVObject>>& children = fogOfWarOverlay->GetChildren();

						for (const auto& child : children)
						{
							int stop = 1;
						}
					}

					RE::RenderPassCache* renderPassCache = RE::RenderPassCache::GetSingleton();

					RE::RenderPassCache::Pool& pool = renderPassCache->pools[0];

					static bool shown;

					auto firstOfAll = reinterpret_cast<uint64_t>(&pool.passes[0]);
					auto lastOfAll = reinterpret_cast<uint64_t>(&pool.passes[65534]);

					if (!shown)
					{
						logger::debug("POOL:   First:0x{:08x}, Last:0x{:08x}", firstOfAll, lastOfAll);
						shown = true;
					}

					RE::BSSpinLockGuard lockGuard(pool.lock);

					auto firstAvailableBefore = reinterpret_cast<uint64_t>(pool.firstAvailable);
					auto lastAvailableBefore = reinterpret_cast<uint64_t>(pool.lastAvailable);

					logger::debug("BEFORE: First:0x{:08x}, Last:0x{:08x}", firstAvailableBefore, lastAvailableBefore);

					//localMap->localCullingProcess.Setup();
					localMap->localCullingProcess.RenderOffScreen();

					auto firstAvailableAfter = reinterpret_cast<uint64_t>(pool.firstAvailable);
					auto lastAvailableAfter = reinterpret_cast<uint64_t>(pool.lastAvailable);

					logger::debug("AFTER:  First:0x{:08x}, Last:0x{:08x}", firstAvailableAfter, lastAvailableAfter);
				}
			}
		}
	//	else
	//	{
	//		miniMap->RemoveLocalMap();
	//	}
	}

	hooks::HUDMenu::PreDisplay(a_hudMenu);
}

void LocalMapMenuTest(RE::LocalMapMenu* a_localMapMenu, const char* a_fileName)
{
	RE::BSFixedString fileName = a_fileName;

	a_localMapMenu->GetRuntimeData().imageData.BindToVirtualTarget(fileName, RE::RENDER_TARGET::kLOCAL_MAP);

	RE::NiPoint3 maxExtentExtra{ 0, 0, 40000 };

	auto localMapCamera = a_localMapMenu->localCullingProcess.GetLocalMapCamera();
	localMapCamera->defaultState->minFrustumHalfWidth = 4000.0F * 0.5F;
	localMapCamera->defaultState->minFrustumHalfHeight = localMapCamera->defaultState->minFrustumHalfWidth * 9.0F / 16.0F;
	a_localMapMenu->localCullingProcess.Setup();
}

// This function is here only for testing
bool WorldPtToScreenPt3(RE::NiCamera* a_camera, const RE::NiPoint3& a_location, float& a_xOut, float& a_yOut, float& a_zOut, float a_zeroTolerance)
{
	auto playerLocation = RE::PlayerCharacter::GetSingleton()->GetPosition();

	if (playerLocation == a_location)
	{
		auto& runtimeData = a_camera->GetRuntimeData();
		auto& runtimeData2 = a_camera->GetRuntimeData2();
		
		if (auto mapMenu = static_cast<RE::MapMenu*>(RE::UI::GetSingleton()->GetMenu(RE::MapMenu::MENU_NAME).get()))
		{
			RE::LocalMapCamera* localMapCamera = mapMenu->GetRuntimeData().localMapMenu.localCullingProcess.GetLocalMapCamera();
			RE::LocalMapCamera* miniMapCamera = MiniMap::GetSingleton()->GetLocalMap()->localCullingProcess.GetLocalMapCamera();

			int stop = 1;
		}
	}
	return hooks::NiCamera::WorldPtToScreenPt3(a_camera, a_location, a_xOut, a_yOut, a_zOut, a_zeroTolerance);
}