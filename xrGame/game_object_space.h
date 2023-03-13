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

#ifdef MOUSE_CALLBACKS
		eOnMouseWheel,
		eOnMouseMove,
		eOnMousePress,
		eOnMouseRelease,
		eOnMouseHold,
#endif

#ifdef KEYBOARD_CALLBACKS
		eOnKeyPress,
		eOnKeyRelease,
		eOnKeyHold,
#endif

		eActorSleep,

#ifdef WEAPON_CALLBACKS
		eOnActorWeaponFire,
		eOnActorWeaponJammed,
		eOnActorWeaponEmpty,
		eOnActorWeaponReload,
		eOnActorWeaponZoomIn,
		eOnActorWeaponZoomOut,

		eOnNPCWeaponFire,
		eOnNPCWeaponJammed,
		eOnNPCWeaponEmpty,
		eOnNPCWeaponReload,
#endif

		eHelicopterOnPoint,
		eHelicopterOnHit,

		eOnItemTake,
		eOnItemDrop,

#ifdef ITEM_MOVEMENT_CALLBACKS
		eOnItemRuck,
		eOnItemSlot,
		eOnItemBelt,
#endif

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