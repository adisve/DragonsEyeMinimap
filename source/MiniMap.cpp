#include "MiniMap.h"

bool MiniMap::ProcessMessage(RE::UIMessage* a_message)
{
	if (RE::LocalMapMenu* localMap = GetLocalMap())
	{
		//if (!holder->GetRuntimeData().movieView)
		{
			localMap->InitScaleform(view.get());
		}

		//localMap->GetRuntimeData().enabled = false;
		localMap->Show(true);

		RE::NiPoint3 maxExtentExtra{ 0, 0, 40000 };

		auto localMapCamera = localMap->localCullingProcess.GetLocalMapCamera();
		localMapCamera->minExtent = loadedAreaBound->minExtent;
		localMapCamera->maxExtent = loadedAreaBound->maxExtent + maxExtentExtra;
	}

	return false;
}

void MiniMap::SetLocalMapExtentsImpl(const RE::FxDelegateArgs& a_delegateArgs)
{
	float localLeft = a_delegateArgs[0].GetNumber();
	float localTop = a_delegateArgs[1].GetNumber();
	float localRight = a_delegateArgs[2].GetNumber();
	float localBottom = a_delegateArgs[3].GetNumber();

	RE::LocalMapMenu* localMap = GetLocalMap();

	float identityMat2D[2][3] = { { 1.0F, 0.0F, 0.0F }, { 0.0F, 1.0F, 0.0F } };

	RE::GPointF localTopLeft{ localLeft, localTop };
	localMap->topLeft = view->TranslateToScreen(localTopLeft, identityMat2D);

	RE::GPointF localBottomRight{ localRight, localBottom };
	localMap->bottomRight = view->TranslateToScreen(localBottomRight, identityMat2D);
}