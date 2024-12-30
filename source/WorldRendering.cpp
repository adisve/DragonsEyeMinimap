#include "Minimap.h"

#include "RE/B/BSTimer.h"
#include "RE/I/ImageSpaceManager.h"
#include "RE/I/ImageSpaceShaderParam.h"
#include "RE/L/LocalMapCamera.h"
#include "RE/R/Renderer.h"
#include "RE/S/ShaderAccumulator.h"
#include "RE/S/ShadowState.h"
#include "RE/R/RenderPassCache.h"
#include "RE/R/RenderTargetManager.h"

namespace RE
{
	class BSPortalGraphEntry : public NiRefObject
	{
	public:
		~BSPortalGraphEntry() override;	 // 00

		// members
		BSPortalGraph* portalGraph;	 // 10
									 // ...
	};
	//static_assert(sizeof(BSPortalGraphEntry) == 0x140);

	RE::BSPortalGraphEntry* Main__GetPortalGraphEntry(Main* a_this)
	{
		using func_t = decltype(&Main__GetPortalGraphEntry);
		REL::Relocation<func_t> func{ RELOCATION_ID(35607, 36617) };

		return func(a_this);
	}

	void* NiCamera__Accumulate(NiCamera* a_this, BSGraphics::BSShaderAccumulator* a_shaderAccumulator, std::uint32_t a_unk2)
	{
		using func_t = decltype(&NiCamera__Accumulate);
		REL::Relocation<func_t> func{ RELOCATION_ID(99789, 106436) };

		return func(a_this, a_shaderAccumulator, a_unk2);
	}
}

namespace DEM
{
	void Minimap::UpdateFogOfWar()
	{
		auto& fogOfWarOverlayHolder = reinterpret_cast< RE::NiPointer<RE::NiNode>&>(cullingProcess->GetFogOfWarOverlay());

		if (!fogOfWarOverlayHolder)
		{
			localMap->localCullingProcess.CreateFogOfWar();
		}
		else
		{
			// Clear last-frame fog of war overlay because I don't know how to update BSTriShapes.
			// Maybe that would improve performance.
			std::uint32_t childrenSize = fogOfWarOverlayHolder->GetChildren().size();
			for (std::uint32_t i = 0; i < childrenSize; i++)
			{
				fogOfWarOverlayHolder->DetachChildAt(i);
			}

			RE::LocalMapMenu::FogOfWar fogOfWar;

			fogOfWar.overlayHolder = fogOfWarOverlayHolder.get();

			RE::TESObjectCELL* interiorCell = RE::TES::GetSingleton()->interiorCell;

			if (interiorCell)
			{
				fogOfWar.minExtent = cameraContext->minExtent;
				fogOfWar.maxExtent = cameraContext->maxExtent;
				fogOfWar.gridDivisions = 32;
		
				cullingProcess->AttachFogOfWarOverlay(fogOfWar, interiorCell);
			}
			else
			{
				fogOfWar.gridDivisions = 16;
		
				RE::GridCellArray* gridCells = RE::TES::GetSingleton()->gridCells;
				std::uint32_t gridLength = gridCells->length;
		
				if (gridLength)
				{
					for (std::uint32_t gridX = 0; gridX < gridLength; gridX++)
					{	
						for (std::uint32_t gridY = 0; gridY < gridLength; gridY++)
						{					
							RE::TESObjectCELL* cell = gridCells->GetCell(gridX, gridY);
							if (cell && cell->IsAttached())
							{
								cullingProcess->AttachFogOfWarOverlay(fogOfWar, cell);
							}
						}
					}
				}

				RE::TESWorldSpace* worldSpace = RE::TES::GetSingleton()->GetRuntimeData2().worldSpace;
				if (worldSpace)
				{
					RE::TESObjectCELL* skyCell = worldSpace->GetSkyCell();
					if (skyCell && skyCell->IsAttached())
					{
						cullingProcess->AttachFogOfWarOverlay(fogOfWar, skyCell);
					}
				}
			}
	
			fogOfWarOverlayHolder->local.translate.z = (cameraContext->maxExtent.z - cameraContext->minExtent.z) * 0.5;
			
			RE::NiUpdateData niUpdateData;
			niUpdateData.time = 0.0F;

			fogOfWarOverlayHolder->Update(niUpdateData);
		}
	}

	void Minimap::RenderOffScreen()
	{
		LMU::PixelShaderProperty::Shape prevShaderShape;
		LMU::PixelShaderProperty::Style shaderStyle;

		GetPixelShaderProperties(prevShaderShape, shaderStyle);
		SetPixelShaderProperties(shape, shaderStyle);

		// 1. Setup culling step ///////////////////////////////////////////////////////////////////////////////////////////

		useMapBrightnessAndContrastBoost = true;

		RE::ShadowSceneNode* mainShadowSceneNode = RE::ShadowSceneNode::GetMain();

        RE::NiPointer<RE::BSGraphics::BSShaderAccumulator>& shaderAccumulator = cullingProcess->GetShaderAccumulator();

		shaderAccumulator->GetRuntimeData().activeShadowSceneNode = mainShadowSceneNode;

		RE::NiTObjectArray<RE::NiPointer<RE::NiAVObject>>& mainShadowSceneChildren = mainShadowSceneNode->GetChildren();

		bool isLightUpdateDisabled = mainShadowSceneNode->GetRuntimeData().disableLightUpdate;
		mainShadowSceneNode->GetRuntimeData().disableLightUpdate = true;

		RE::NiPointer<RE::NiAVObject>& objectLODRoot = mainShadowSceneChildren[3];
		bool areLODsHidden = objectLODRoot->GetFlags().any(RE::NiAVObject::Flag::kHidden);
		objectLODRoot->GetFlags().reset(RE::NiAVObject::Flag::kHidden);

		bool isByte_1431D1D30 = byte_1431D1D30;
		bool isNodeFadeEnabled = nodeFadeEnabled;
		byte_1431D1D30 = true;
		nodeDrawFadeEnabled = nodeFadeEnabled = false;
		dword_1431D0D8C = 0;

		RE::BSGraphics::Renderer* renderer = RE::BSGraphics::Renderer::GetSingleton();
		renderer->SetClearColor(0.0F, 0.0F, 0.0F, 1.0F);

        RE::TES* tes = RE::TES::GetSingleton();
		RE::TESWorldSpace* worldSpace = tes->GetRuntimeData2().worldSpace;

		RE::LocalMapMenu::LocalMapCullingProcess::UnkData unkData{ cullingProcess };
		
		if (worldSpace && worldSpace->flags.any(RE::TESWorldSpace::Flag::kFixedDimensions))
		{
			unkData.unk8 = false;
		}
		else 
		{
			unkData.unk8 = true;
		}


		// 2. Culling step /////////////////////////////////////////////////////////////////////////////////////////////////

		RE::TESObjectCELL* currentCell = nullptr;

		if (RE::TESObjectCELL* interiorCell = tes->interiorCell)
		{
			currentCell = interiorCell;
		}
		else if (worldSpace)
		{
			RE::TESObjectCELL* skyCell = worldSpace->GetSkyCell();
			if (skyCell && skyCell->IsAttached())
            {
				currentCell = skyCell;

				CullTerrain(tes->gridCells, unkData, nullptr);
            }
		}

        if (currentCell)
        {
			cullingProcess->CullCellObjects(unkData, currentCell);
		}

        RE::CullJobDescriptor& cullJobDesc = cullingProcess->cullJobDesc;
		RE::NiPointer<RE::NiCamera> camera = cameraContext->camera;

        cullJobDesc.camera = camera;

        RE::BSPortalGraphEntry* portalGraphEntry = RE::Main__GetPortalGraphEntry(RE::Main::GetSingleton());
		
        if (portalGraphEntry)
        {
			RE::BSPortalGraph* portalGraph = portalGraphEntry->portalGraph;
            if (portalGraph)
            {
				cullJobDesc.cullingObjects = reinterpret_cast<RE::BSTArray<RE::NiPointer<RE::NiAVObject>>*>(&portalGraph->unk58);
				cullJobDesc.Cull(1, 0);
            }
        }

        if (mainShadowSceneChildren.capacity() > 9)
        {
			RE::NiPointer<RE::NiAVObject>& portalSharedNode = mainShadowSceneChildren[9];
			cullJobDesc.scene = portalSharedNode;
        }
		else
		{
			cullJobDesc.scene = nullptr;
		}
		cullJobDesc.Cull(0, 0);

        if (mainShadowSceneChildren.capacity() > 8)
        {
			RE::NiPointer<RE::NiAVObject>& multiBoundNode = mainShadowSceneChildren[8];
			cullJobDesc.scene = multiBoundNode;
        }
		else
		{
			cullJobDesc.scene = nullptr; 
		}
		cullJobDesc.Cull(0, 0);

		// 3. Rendering step ///////////////////////////////////////////////////////////////////////////////////////////////

        RE::BSGraphics::RenderTargetManager* renderTargetManager = RE::BSGraphics::RenderTargetManager::GetSingleton();

        int depthStencil = renderTargetManager->GetDepthStencil();
		renderTargetManager->SetupDepthStencilAt(depthStencil, RE::BSGraphics::SetRenderTargetMode::SRTM_CLEAR, 0, false);
		renderTargetManager->SetupRenderTargetAt(0, RE::RENDER_TARGET::kLOCAL_MAP_SWAP, RE::BSGraphics::SetRenderTargetMode::SRTM_CLEAR, true);

		RE::NiCamera__Accumulate(camera.get(), shaderAccumulator.get(), 0);

		// 4. Post process step (Add fog of war) ///////////////////////////////////////////////////////////////////////////

		if (isFogOfWarEnabled)
		{
			shaderAccumulator->ClearActiveRenderPasses(false);

			RE::BSGraphics::BSShaderAccumulator::SetRenderMode(19);

			RE::NiPointer<RE::NiAVObject> fogOfWarOverlayHolder = cullingProcess->GetFogOfWarOverlay();
			cullJobDesc.scene = fogOfWarOverlayHolder;
			cullJobDesc.Cull(0, 0);

			RE::BSGraphics::RendererShadowState* rendererShadowState = RE::BSGraphics::RendererShadowState::GetSingleton();

			rendererShadowState->SetAlphaBlendWriteMode(8);
			rendererShadowState->SetDepthStencilDepthMode(RE::BSGraphics::DepthStencilDepthMode::kDisabled);

			renderTargetManager->SetupRenderTargetAt(0, RE::RENDER_TARGET::kLOCAL_MAP_SWAP, RE::BSGraphics::SetRenderTargetMode::SRTM_RESTORE, true);
			RE::NiCamera__Accumulate(camera.get(), shaderAccumulator.get(), 0);

			rendererShadowState->SetDepthStencilDepthMode(RE::BSGraphics::DepthStencilDepthMode::kTestWrite);
			rendererShadowState->SetAlphaBlendWriteMode(1);
		}

		// 5. Finish rendering and dispatch ////////////////////////////////////////////////////////////////////////////////

        renderTargetManager->SetupDepthStencilAt(-1, RE::BSGraphics::SetRenderTargetMode::SRTM_RESTORE, 0, false);

		RE::ImageSpaceShaderParam& imageSpaceShaderParam = cullingProcess->GetImageSpaceShaderParam();

		RE::BSGraphics::RenderTargetProperties& renderLocalMapSwapData = renderTargetManager->renderTargetData[RE::RENDER_TARGET::kLOCAL_MAP_SWAP];

		float localMapSwapWidth = renderLocalMapSwapData.width;
		float localMapSwapHeight = renderLocalMapSwapData.height;

		imageSpaceShaderParam.SetupPixelConstantGroup(0, 1.0 / localMapSwapWidth, 1.0 / localMapSwapHeight, 0.0, 0.0);
		
        RE::ImageSpaceManager* imageSpaceManager = RE::ImageSpaceManager::GetSingleton();

		imageSpaceManager->CopyWithImageSpaceEffect(RE::ImageSpaceManager::ImageSpaceEffectEnum::ISLocalMap,
													RE::RENDER_TARGET::kLOCAL_MAP_SWAP, RE::RENDER_TARGET::kLOCAL_MAP,
													&imageSpaceShaderParam);

		if (areLODsHidden)
		{
			objectLODRoot->GetFlags().set(RE::NiAVObject::Flag::kHidden);
		}

		mainShadowSceneNode->GetRuntimeData().disableLightUpdate = isLightUpdateDisabled;
		byte_1431D1D30 = isByte_1431D1D30;
		nodeDrawFadeEnabled = nodeFadeEnabled = isNodeFadeEnabled;
		dword_1431D0D8C = 0;

        shaderAccumulator->ClearActiveRenderPasses(false);

		useMapBrightnessAndContrastBoost = false;

		SetPixelShaderProperties(prevShaderShape, shaderStyle);
	}

	// Terrain render passes can be allocated multiple times but only cleared once per frame.
	// I don't understand why only terrain render passes work this way, but once Draw is called
	// before entering my code, make sure we clear them so there is no memory leakage.
	void Minimap::ClearTerrainRenderPasses(RE::NiPointer<RE::NiAVObject>& a_object)
	{
		RE::NiNode* node = a_object->AsNode();

		if (!node)
		{
			return;
		}

		for (RE::NiPointer<RE::NiAVObject>& object : node->GetChildren()) if (object)
		{
			if (object->GetFlags().any(RE::NiAVObject::Flag::kRenderUse))
			{
				if (RE::BSGeometry* geometry = object->AsGeometry())
				{
					auto shaderProp = static_cast<RE::BSShaderProperty*>(geometry->GetGeometryRuntimeData().properties[RE::BSGeometry::States::kEffect].get());
					if (shaderProp)
					{
						shaderProp->DoClearRenderPasses();
					}
				}
			}

			ClearTerrainRenderPasses(object);
		}
	};

	void Minimap::CullTerrain(const RE::GridCellArray* a_gridCells, RE::LocalMapMenu::LocalMapCullingProcess::UnkData& a_unkData,
							  const RE::TESObjectCELL* a_cell)
	{
		RE::CullJobDescriptor& cullJobDesc = a_unkData.ptr->cullJobDesc;

		for (int gridCellX = 0; gridCellX < a_gridCells->length; gridCellX++)
		{
			for (int gridCellY = 0; gridCellY < a_gridCells->length; gridCellY++)
			{
				RE::TESObjectCELL* cell = a_gridCells->GetCell(gridCellX, gridCellY);
				if (cell && cell->IsAttached())
				{
					for (int sceneIndex : { 2, 3, 6, 7 }) if (sceneIndex != 2 || a_unkData.unk8)
					{
						if (RE::NiPointer<RE::NiNode> cell3D = cell->GetRuntimeData().loadedData->cell3D)
						{
							RE::NiTObjectArray<RE::NiPointer<RE::NiAVObject>>& cellScenes = cell3D->GetChildren();

							if (sceneIndex < cellScenes.size())
							{
								if (RE::NiPointer<RE::NiAVObject> scene = cellScenes[sceneIndex])
								{
									if (sceneIndex == 2)
									{
										ClearTerrainRenderPasses(scene);
									}

									bool isCellSceneHidden = scene->GetFlags().any(RE::NiAVObject::Flag::kHidden);
									scene->GetFlags().reset(RE::NiAVObject::Flag::kHidden);

									cullJobDesc.scene = scene;

									cullJobDesc.Cull(0, 0);

									if (isCellSceneHidden)
									{
										scene->GetFlags().set(RE::NiAVObject::Flag::kHidden);
									}
								}
							}
						}
					}
				}
			}
		}

		cullJobDesc.scene = nullptr;
	}
}