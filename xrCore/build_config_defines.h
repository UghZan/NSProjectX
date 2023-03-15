//used mostly for some of the changes that are non-configurable through sections and could be detrimental to loading times/performance/user experience

// ~~~CALLBACKS~~~
//#define MOUSE_CALLBACKS //mouse hold/move/release callbacks
//#define KEYBOARD_CALLBACKS //key press/hold/release callbacks
//#define ITEM_MOVEMENT_CALLBACKS //callbacks for moving items to ruck/belt/slots	
//#define WEAPON_CALLBACKS //fire/empty/jam/reload callbacks

// ~~~GAMEPLAY~~~
#define SIMPLIFIED_FIRE_WOUND_PROTECTION //fire_wound no longer uses bone_protection of outfit, instead using immunity value

// ~~~DEV~~~
#define LUAICP_COMPAT //lua interceptor compat by alpet from xp-dev_xray
//#define SCRIPT_ICONS_CONTROL //stop engine-side warning indicators (like radiation, godmode, weapon condition etc.) from showing
//#define SCRIPT_EZI_CONTROL //allows control of zoom inertion from scripts
#define ARTEFACTS_WORK_IN_RUCK //artefacts have an influence on actor even if in ruck, with configurable intensity