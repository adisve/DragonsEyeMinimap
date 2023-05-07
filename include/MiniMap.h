#pragma once

#include "IUI/GFxDisplayObject.h"

#include "RE/B/BSScaleformManager.h"
#include "RE/B/BSScaleformImageLoader.h"
#include "RE/H/HUDObject.h"
#include "RE/L/LocalMapMenu.h"
#include "RE/L/LoadedAreaBound.h"

class MiniMap : public RE::HUDObject
{
public:

	// override HUDObject
	void Update() override { };								 // 01
	bool ProcessMessage(RE::UIMessage* a_message) override;	 // 02 - { return false; }

	static void InitSingleton(const IUI::GFxDisplayObject& a_map)
	{
		if (!singleton)
		{
			static MiniMap singletonInstance{ a_map };
			singleton = &singletonInstance;
		}
	}

	static MiniMap* GetSingleton() { return singleton; }

	static void SetLocalMapExtents(const RE::FxDelegateArgs& a_delegateArgs)
	{
		singleton->SetLocalMapExtentsImpl(a_delegateArgs);
	}

	bool InitLocalMap()
	{
		if (!localMap)
		{
			if (loadedAreaBound = reinterpret_cast<RE::LoadedAreaBound*>(RE::TES::GetSingleton()->unk2B0))
			{
				if (loadedAreaBound->maxExtent.x)
				{
					struct LocalMapMenuBytes
					{
						std::uint8_t bytes[sizeof(RE::LocalMapMenu)];
					};

					localMap = reinterpret_cast<RE::LocalMapMenu*>(new LocalMapMenuBytes);
					localMap->Ctor();

					return true;
				}
			}
		}

		return false;
	}

	RE::LocalMapMenu* GetLocalMap() { return localMap; }

	void RemoveLocalMap()
	{
		if (localMap)
		{
			struct LocalMapMenuBytes
			{
				std::uint8_t bytes[sizeof(RE::LocalMapMenu)];
			};

			localMap->Dtor();
			delete reinterpret_cast<LocalMapMenuBytes*>(localMap);
			localMap = nullptr;
		}
	}

private:

	MiniMap(const IUI::GFxDisplayObject& a_map) :
		HUDObject{ a_map.GetMovieView() }
	{}
	
	void SetLocalMapExtentsImpl(const RE::FxDelegateArgs& a_delegateArgs);

	static inline MiniMap* singleton = nullptr;

	RE::LoadedAreaBound* loadedAreaBound;

	RE::LocalMapMenu* localMap;
};