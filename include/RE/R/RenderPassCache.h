#pragma once

namespace RE
{
	class RenderPassCache
	{
	public:

		struct Pool
		{
			BSRenderPass* passes;
			BSLight** lights;
			BSRenderPass* firstAvailable;
			BSRenderPass* lastAvailable;
			BSSpinLock lock;
		};

		static RenderPassCache* GetSingleton()
		{
			static const REL::Relocation<RenderPassCache*> singleton(RELOCATION_ID(528206, 415150));

			return singleton.get();
		}

		Pool pools[2];
	};
}