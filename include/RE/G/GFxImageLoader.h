#pragma once

#include "RE/G/GFxState.h"

namespace RE
{
	class GImageInfoBase;

	class GFxImageLoader : public GFxState
	{
	public:

		~GFxImageLoader() override = default;  // 00

		// add
		virtual GImageInfoBase* LoadImage(const char* a_imageUrl) = 0; // 01
	};
	static_assert(sizeof(GFxImageLoader) == 0x18);
}