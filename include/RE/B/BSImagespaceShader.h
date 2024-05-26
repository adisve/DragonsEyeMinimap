#pragma once

#include "RE/B/BSShader.h"
#include "RE/I/ImageSpaceEffect.h"

#include "RE/N/NiRefObject.h"

namespace RE
{
	class BSComputeShader :
		public NiRefObject,
		public BSReloadShaderI
	{
	public:
		inline static constexpr auto RTTI = RTTI_BSComputeShader;

		// members
		std::uint64_t unk18;  // 18
		std::uint64_t unk20;  // 20
		std::uint64_t unk28;  // 28
		std::uint64_t unk30;  // 30
		std::uint64_t unk38;  // 38
		std::uint64_t unk40;  // 40
		std::uint64_t unk48;  // 48
	};
	static_assert(sizeof(BSComputeShader) == 0x50);

	class BSImagespaceShader :
		public BSShader,		 // 000
		public ImageSpaceEffect	 // 090
	{
	public:
		inline static constexpr auto RTTI = RTTI_BSImagespaceShader;

		~BSImagespaceShader() override;	 // 00

		// override (BSShader)
		void Setup(ImageSpaceManager* a_manager, ImageSpaceEffectParam* a_param) override;	// 02
		void ShutDown() override;															// 03
		bool IsActive() override;															// 06
		bool UpdateParams(ImageSpaceEffectParam* a_param) override;							// 07
		bool RestoreRenderStates(ImageSpaceEffectParam* a_param) override;					// 09

		// add
		virtual void Unk_0A(void);	// 0A - { return; }
		virtual void Unk_0B(void);	// 0B - { return; }
		virtual void Unk_0C(void);	// 0C
		virtual void Unk_0D(void);	// 0D
		virtual void Unk_0E(void);	// 0E

		// members
		BSFixedString name;				 // 120 "BSImagespaceShaderISTemporalAA"
		BSFixedString type;				 // 128 "ISTemporalAA"
		std::uint64_t unk130;			 // 130
		std::uint64_t unk138;			 // 138
		std::uint64_t unk140;			 // 140
		std::uint64_t unk148;			 // 148
		std::uint64_t unk150;			 // 150
		std::uint64_t unk158;			 // 158
		std::uint64_t unk160;			 // 160
		std::uint64_t unk168;			 // 168
		std::uint64_t unk170;			 // 170
		std::uint64_t unk178;			 // 178
		std::uint64_t unk180;			 // 180
		std::uint64_t unk188;			 // 188
		std::uint64_t unk190;			 // 190
		BSComputeShader* computeShader;	 // 198
		std::uint64_t unk1A0;			 // 1A0
	};
	static_assert(sizeof(BSImagespaceShader) == 0x1A8);
}
