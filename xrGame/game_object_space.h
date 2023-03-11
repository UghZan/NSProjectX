#pragma once

namespace GameObject {
	enum ECallbackType {
		eTradeStart = u32(0),
		eTradeStop,
		eTradeSellBuyItem,
		eTradePerformTradeOperation,

		eZoneEnter,
		eZoneExit,
		eExitLevelBorder,
		eEnterLevelBorder,
		eDeath,

		ePatrolPathInPoint,

		eInventoryPda,
		eInventoryInfo,
		eArticleInfo,
		eTaskStateChange,
		eMapLocationAdded,

		eUseObject,

		eHit,

		eSound,

		eActionTypeMovement,
		eActionTypeWatch,
		eActionTypeAnimation,
		eActionTypeSound,
		eActionTypeParticle,
		eActionTypeObject,

		eOnMouseWheel,
		eOnMouseMove,
		eOnKeyPress,
		eOnKeyRelease,
		eOnKeyHold,

		eActorSleep,

		eHelicopterOnPoint,
		eHelicopterOnHit,

		eOnItemTake,
		eOnItemDrop,
		eOnItemRuck,
		eOnItemSlot,
		eOnItemBelt,

		eScriptAnimation,

		eSwitchTorch,
		eSwitchNV,
		
		eTraderGlobalAnimationRequest,
		eTraderHeadAnimationRequest,
		eTraderSoundEnd,

		eInvBoxItemTake,
		eDummy = u32(-1),
	};
};

enum EBoneCallbackType{
	bctDummy			= u32(0),	// 0 - required!!!
	bctPhysics,
	bctCustom,
	bctForceU32			= u32(-1),
};