#include	"stdafx.h"
#pragma		hdrstop

#include	"xrRender_console.h"

u32			ps_Preset = 3;
xr_token							qpreset_token[] = {
	{ "Minimum",					0											},
	{ "Low",						1											},
	{ "Default",					2											},
	{ "High",						3											},
	{ "Maximum",					4											},
	{ "Extreme",					5											},
	{ "Ultra",						6											},
	{ 0,							0											}
};

u32			ps_EffPreset = 2;
xr_token							qeffpreset_token[] = {
	{ "st_opt_eff_disabled",		0											},
	{ "st_opt_eff_default",			1											},
	{ "st_opt_eff_cinematic",		2											},
	{ 0,							0											}
};

u32		ps_aa = 1;
xr_token							aa_token[] = {
	{ "st_opt_off",					1											},
	{ "st_opt_dlaa",				2											},
	{ "st_opt_dlaa_force_edge_detect", 3										},
	{ "st_opt_fxaa",				4											},
	{ 0,							0											} 
};

u32		ps_aa_quality = 2;
xr_token aa_quality_token[] = {
	{ "st_opt_low",					1											},
	{ "st_opt_medium",				2											},
	{ "st_opt_high",				3											},
	{ "st_opt_ultra",				4											},
	{ 0,							0											} 
};

u32		ps_ao = 2;
xr_token ao_token[] = {
	{ "st_opt_off",						0											},
	{ "st_opt_ssao",				1											},
	{ "st_opt_hdao",				2											},
	{ "st_opt_hbao",				3											},
//	{ "st_opt_gtao",				4											},
	{ 0,							0											} 
};

u32		ps_ao_quality = 2;
xr_token ao_quality_token[] = {
	{ "st_opt_low",					1											},
	{ "st_opt_medium",				2											},
	{ "st_opt_high",				3											},
	{ "st_opt_ultra",				4											},
	{ 0,							0											} 
};

u32		ps_sun_quality = 1;
xr_token sun_quality_token[] = {
	{ "st_opt_low",					1											},
	{ "st_opt_medium",				2											},
	{ "st_opt_high",				3											},
	{ "st_opt_extreme",				4											},
	{ "st_opt_ultra",				5											},
	{ 0,							0											} 
};

u32		ps_shadow_filtering = 1;
xr_token shadow_filter_token[] = {
	{ "st_opt_disable",				0											},
	{ "st_opt_min",					1											},
	{ "st_opt_mid",					2											},
	{ "st_opt_max",					3											},
	{ 0,							0											} 
};

u32		ps_debug_frame_layers = 0;
xr_token debug_frame_layers_token[] = {
	{ "full_frame",					0											},
	{ "gbuffer_color",				1											},
	{ "gbuffer_position",			2											},
	{ "gbuffer_normal",				3											},
	{ "gbuffer_gloss",				4											},
	{ "gbuffer_hemisphere",			5											},
	{ "gbuffer_material",			6											},
	{ "gbuffer_split",				7											},
	{ "accumulator_light",			8											},
	{ "accumulator_specular",		9											},
	{ "real_time_ao",				10											},
	{ "real_time_ao_with_hemi",		11											},
	{ "split_with_real_time_ao_and_without", 12									},
	{ "split_with_hemisphere_and_without",	 13									},
	{ "split_ao_blur_stages",				 14									},
	{ "full_ao_with_light",			15											},
	{ 0,							0											}
};

u32		ps_debug_textures = 0;
xr_token ps_debug_textures_token[] = {
	{ "disabled",					0											},
	{ "uv_checker",					1											},
	{ "white",						2											},
	{ 0,							0											}
};

u32		ps_bump_mode = 2;
xr_token bump_mode_token[] = {
	{ "st_opt_normal_mapping",		1											},
	{ "st_opt_parallax_mapping",	2											},
	{ "st_opt_steep_parallax_mapping",3											},
//	{ "st_opt_parallax_occlusion_mapping",4										},
	{ 0,							0											}
};

u32		ps_tdetail_bump_mode = 2;
xr_token tdetail_bump_mode_token[] = {
	{ "st_opt_normal_mapping",		1											},
	{ "st_opt_parallax_mapping",	2											},
	{ "st_opt_steep_parallax_mapping",3											},
//	{ "st_opt_parallax_occlusion_mapping",4										},
	{ 0,							0											}
};

u32		ps_terrain_bump_mode = 2;
xr_token terrain_bump_mode_token[] = {
	{ "st_opt_normal_mapping",		1											},
	{ "st_opt_parallax_mapping",	2											},
	{ "st_opt_steep_parallax_mapping",3											},
	{ 0,							0											}
};

u32			ps_r_sun_shafts = 2;
xr_token							qsun_shafts_token[] = {
	{ "st_opt_off",					0												},
	{ "st_opt_low",					1												},
	{ "st_opt_medium",				2												},
	{ "st_opt_high",				3												},
	{ 0,							0												}
};

u32		ps_dof_quality = 2;
xr_token dof_quality_token[] = {
	{ "st_opt_low",					1											},
	{ "st_opt_medium",				2											},
	{ "st_opt_high",				3											},
	{ "st_opt_ultra",				4											},
	{ 0,							0											}
};

// KD start
Flags32		ps_common_flags = { 0 };		// r1-only
u32			ps_steep_parallax = 0;
int			ps_r__detail_radius = 49;
u32			dm_size = 24;
u32 		dm_cache1_line = 12;	//dm_size*2/dm_cache1_count
u32			dm_cache_line = 49;	//dm_size+1+dm_size
u32			dm_cache_size = 2401;	//dm_cache_line*dm_cache_line
float		dm_fade = 47.5;	//float(2*dm_size)-.5f;
u32			dm_current_size = 24;
u32 		dm_current_cache1_line = 12;	//dm_current_size*2/dm_cache1_count
u32			dm_current_cache_line = 49;	//dm_current_size+1+dm_current_size
u32			dm_current_cache_size = 2401;	//dm_current_cache_line*dm_current_cache_line
float		dm_current_fade = 47.5;	//float(2*dm_current_size)-.5f;
float		ps_current_detail_density = 0.6;
// KD end

// Common
//int		ps_r__Supersample			= 1		;
int			ps_r__LightSleepFrames = 10;

float		ps_r__Detail_l_ambient = 0.9f;
float		ps_r__Detail_l_aniso = 0.25f;
float		ps_r__Detail_density = 0.3f;
float		ps_r__Detail_rainbow_hemi = 0.75f;
float		ps_current_detail_scale = 1.f;

float		ps_r__Tree_w_rot = 10.0f;
float		ps_r__Tree_w_speed = 1.00f;
float		ps_r__Tree_w_amp = 0.005f;
Fvector		ps_r__Tree_Wave = { .1f, .01f, .11f };
float		ps_r__Tree_SBC = 1.5f;	// scale bias correct

float		ps_r__WallmarkTTL = 300.f;
float		ps_r__WallmarkSHIFT = 0.0001f;
float		ps_r__WallmarkSHIFT_V = 0.0001f;

float		ps_r__GLOD_ssa_start = 256.f;
float		ps_r__GLOD_ssa_end = 64.f;
float		ps_r__LOD = 1.f;
//. float		ps_r__LOD_Power				=  1.5f	;
float		ps_r__ssaDISCARD = 3.5f;					//RO
float		ps_r__ssaDONTSORT = 32.f;					//RO
float		ps_r__ssaHZBvsTEX = 96.f;					//RO

int			ps_r__tf_Anisotropic = 4;

// R1
float		ps_r1_ssaLOD_A = 64.f;
float		ps_r1_ssaLOD_B = 48.f;
float		ps_r1_tf_Mipbias = 0.0f;
Flags32		ps_r1_flags = { R1FLAG_DLIGHTS };		// r1-only
float		ps_r1_lmodel_lerp = 0.1f;
float		ps_r1_dlights_clip = 30.f;
float		ps_r1_pps_u = 0.f;
float		ps_r1_pps_v = 0.f;

// R1-specific
int			ps_r1_GlowsPerFrame = 16;					// r1-only

// R2
float		ps_r2_ssaLOD_A = 48.f;
float		ps_r2_ssaLOD_B = 32.f;
float		ps_r2_tf_Mipbias = 0.0f;

// R2-specific
Flags32 ps_r2_ls_flags =
{
	R2FLAG_SUN						|
	R2FLAG_SUN_IGNORE_PORTALS		|
	R2FLAG_EXP_DONT_TEST_UNSHADOWED |
	R2FLAG_USE_NVSTENCIL			|
	R2FLAG_EXP_SPLIT_SCENE			|
	R2FLAG_EXP_MT_CALC				|
	R2FLAG_TONEMAP					|
	R2FLAG_SUN_FOCUS				|
	R2FLAG_SUN_TSM
}; // r2-only

Flags32 ps_r2_pp_flags =
{
	R2FLAG_DOF |
	R2FLAG_MBLUR
}; // r2.5-only

Flags32		ps_r2_ls_flags_ext = { R2FLAGEXT_AO_BLUR };

float		ps_r2_df_parallax_h = 0.02f;
float		ps_r2_df_parallax_range = 75.f;
float		ps_r2_tonemap_middlegray = 0.25f;			// r2-only
float		ps_r2_tonemap_adaptation = 5.f;				// r2-only
float		ps_r2_tonemap_low_lum = 0.001f;			// r2-only
float		ps_r2_tonemap_amount = 0.5f;				// r2-only
float		ps_r2_ls_bloom_kernel_g = 3.3f;				// r2-only
float		ps_r2_ls_bloom_kernel_b = .7f;				// r2-only
float		ps_r2_ls_bloom_speed = 10.f;				// r2-only
float		ps_r2_ls_bloom_kernel_scale = 1.0f;				// r2-only	// gauss
float		ps_r2_ls_dsm_kernel = .7f;				// r2-only
float		ps_r2_ls_psm_kernel = .7f;				// r2-only
float		ps_r2_ls_ssm_kernel = .7f;				// r2-only
float		ps_r2_ls_bloom_threshold = .3f;				// r2-only
Fvector		ps_r2_aa_barier = { .8f, .1f, 0 };	// r2-only
Fvector		ps_r2_aa_weight = { .25f,.25f,0 };	// r2-only
float		ps_r2_aa_kernel = .5f;				// r2-only
float		ps_r2_mblur = .5f;				// .5f
int			ps_r2_GI_depth = 1;				// 1..5
int			ps_r2_GI_photons = 16;				// 8..64
float		ps_r2_GI_clip = EPS_L;			// EPS
float		ps_r2_GI_refl = .9f;				// .9f
float		ps_r2_ls_depth_scale = 1.00001f;			// 1.00001f
float		ps_r2_ls_depth_bias = -0.0001f;			// -0.0001f
float		ps_r2_ls_squality = 1.0f;				// 1.00f
float		ps_r2_sun_tsm_projection = 0.18f;			// 0.18f
float		ps_r2_sun_tsm_bias = -0.05f;			// 
float		ps_r2_sun_near = 12.f;				// 12.0f
float		ps_r2_sun_far = 150.f;
float		ps_r2_sun_near_border = 1.0f;			// 1.0f
float		ps_r2_sun_depth_far_scale = 1.00000f;			// 1.00001f
float		ps_r2_sun_depth_far_bias = 0.00000f;			// -0.0000f
float		ps_r2_sun_depth_near_scale = 1.00001f;			// 1.00001f
float		ps_r2_sun_depth_near_bias = -0.00004f;		// -0.00005f
float		ps_r2_sun_lumscale = 1.0f;				// 1.0f
float		ps_r2_sun_lumscale_hemi = 1.0f;				// 1.0f
float		ps_r2_sun_lumscale_amb = 1.0f;
float		ps_r2_gmaterial = 0.f;				// 
float		ps_r2_zfill = 0.1f;				// .1f

float		ps_r2_dhemi_sky_scale = 0.08f;				// 1.5f
float		ps_r2_dhemi_light_scale = 0.2f;
float		ps_r2_dhemi_light_flow = 0.1f;
float		ps_r2_dhemi_scale = 1.f;				// 1.5f
int			ps_r2_dhemi_count = 5;				// 5
int			ps_r2_wait_sleep = 0;

float		ps_r2_lt_smooth = 1.f;				// 1.f
float		ps_r2_slight_fade = 1.f;				// 1.f

//	x - min (0), y - focus (1.4), z - max (100)
Fvector3	ps_r2_dof = Fvector3().set(-1.25f, 1.4f, 600.f);
float		ps_r2_dof_sky = 30;				//	distance to sky
float		ps_r2_dof_kernel_size = 5.0f;						//	7.0f


//- Mad Max
float		ps_r2_gloss_factor = 1.0f;
//- Mad Max
#ifndef _EDITOR
#include	"..\xrEngine\xr_ioconsole.h"
#include	"..\xrEngine\xr_ioc_cmd.h"

//-----------------------------------------------------------------------
class CCC_tf_Aniso : public CCC_Integer
{
public:
	void	apply() {
		if (0 == HW.pDevice)	return;
		int	val = *value;	clamp(val, 1, 16);
		for (u32 i = 0; i < HW.Caps.raster.dwStages; i++)
			CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, val));
	}
	CCC_tf_Aniso(LPCSTR N, int* v) : CCC_Integer(N, v, 1, 16) { };
	virtual void Execute(LPCSTR args)
	{
		CCC_Integer::Execute(args);
		apply();
	}
	virtual void	Status(TStatus& S)
	{
		CCC_Integer::Status(S);
		apply();
	}
};
class CCC_tf_MipBias : public CCC_Float
{
public:
	void	apply() {
		if (0 == HW.pDevice)	return;
		for (u32 i = 0; i < HW.Caps.raster.dwStages; i++)
			CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)value)));
	}

	CCC_tf_MipBias(LPCSTR N, float* v) : CCC_Float(N, v, -3.0f, +3.0f) { };
	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute(args);
		apply();
	}
	virtual void	Status(TStatus& S)
	{
		CCC_Float::Status(S);
		apply();
	}
};
class CCC_R2GM : public CCC_Float
{
public:
	CCC_R2GM(LPCSTR N, float* v) : CCC_Float(N, v, 0.f, 4.f) { *v = 0; };
	virtual void	Execute(LPCSTR args)
	{
		if (0 == xr_strcmp(args, "on")) {
			ps_r2_ls_flags.set(R2FLAG_GLOBALMATERIAL, TRUE);
		}
		else if (0 == xr_strcmp(args, "off")) {
			ps_r2_ls_flags.set(R2FLAG_GLOBALMATERIAL, FALSE);
		}
		else {
			CCC_Float::Execute(args);
			if (ps_r2_ls_flags.test(R2FLAG_GLOBALMATERIAL)) {
				static LPCSTR	name[4] = { "oren", "blin", "phong", "metal" };
				float	mid = *value;
				int		m0 = iFloor(mid) % 4;
				int		m1 = (m0 + 1) % 4;
				float	frc = mid - float(iFloor(mid));
				Msg("* material set to [%s]-[%s], with lerp of [%f]", name[m0], name[m1], frc);
			}
		}
	}
};
class CCC_Screenshot : public IConsole_Command
{
public:
	CCC_Screenshot(LPCSTR N) : IConsole_Command(N) { };
	virtual void Execute(LPCSTR args) {
		string_path	name;	name[0] = 0;
		sscanf(args, "%s", name);
		LPCSTR		image = xr_strlen(name) ? name : 0;
		::Render->Screenshot(IRender_interface::SM_NORMAL, image);
	}
};
class CCC_ModelPoolStat : public IConsole_Command
{
public:
	CCC_ModelPoolStat(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		RImplementation.Models->dump();
	}
};
//-----------------------------------------------------------------------
class	CCC_Preset : public CCC_Token
{
public:
	CCC_Preset(LPCSTR N, u32* V, xr_token* T) : CCC_Token(N, V, T) {};

	virtual void	Execute(LPCSTR args) {
		CCC_Token::Execute(args);
		string_path		_cfg;
		string_path		cmd;

		switch (*value) {
		case 0:		strcpy(_cfg, "rspec_minimum.ltx");	break;
		case 1:		strcpy(_cfg, "rspec_low.ltx");		break;
		case 2:		strcpy(_cfg, "rspec_default.ltx");	break;
		case 3:		strcpy(_cfg, "rspec_high.ltx");		break;
		case 4:		strcpy(_cfg, "rspec_maximum.ltx");	break;
		case 5:		strcpy(_cfg, "rspec_extreme.ltx");	break;
		case 6:		strcpy(_cfg, "rspec_ultra.ltx");	break;
		}
		FS.update_path(_cfg, "$game_config$", _cfg);
		strconcat(sizeof(cmd), cmd, "cfg_load", " ", _cfg);
		Console->Execute(cmd);
	}
};
//-----------------------------------------------------------------------
class	CCC_EffPreset : public CCC_Token
{
public:
	CCC_EffPreset(LPCSTR N, u32* V, xr_token* T) : CCC_Token(N, V, T) {};

	virtual void	Execute(LPCSTR args) {
		CCC_Token::Execute(args);
		string_path		_cfg;
		string_path		cmd;

		switch (*value) {
		case 0:		strcpy(_cfg, "eff_disabled.ltx");	break;
		case 1:		strcpy(_cfg, "eff_default.ltx");		break;
		case 2:		strcpy(_cfg, "eff_cinematic.ltx");	break;
		}
		FS.update_path(_cfg, "$game_config$", _cfg);
		strconcat(sizeof(cmd), cmd, "cfg_load", " ", _cfg);
		Console->Execute(cmd);
	}
};
#if RENDER==R_R2
#include "r__pixel_calculator.h"
class CCC_BuildSSA : public IConsole_Command
{
public:
	CCC_BuildSSA(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		r_pixel_calculator	c;
		c.run();
	}
};
#endif

class CCC_DofFar : public CCC_Float
{
public:
	CCC_DofFar(LPCSTR N, float* V, float _min = 0.0f, float _max = 10000.0f)
		: CCC_Float(N, V, _min, _max) {}

	virtual void Execute(LPCSTR args)
	{
		float v = float(atof(args));

		if (v < ps_r2_dof.y + 0.1f)
		{
			char	pBuf[256];
			_snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value greater or equal to r2_dof_focus+0.1");
			Msg("~ Invalid syntax in call to '%s'", cName);
			Msg("~ Valid arguments: %s", pBuf);
			Console->Execute("r2_dof_focus");
		}
		else
		{
			CCC_Float::Execute(args);
			if (g_pGamePersistent)
				g_pGamePersistent->SetBaseDof(ps_r2_dof);
		}
	}

	//	CCC_Dof should save all data as well as load from config
	virtual void	Save(IWriter* F) { ; }
};

class CCC_DofNear : public CCC_Float
{
public:
	CCC_DofNear(LPCSTR N, float* V, float _min = 0.0f, float _max = 10000.0f)
		: CCC_Float(N, V, _min, _max) {}

	virtual void Execute(LPCSTR args)
	{
		float v = float(atof(args));

		if (v > ps_r2_dof.y - 0.1f)
		{
			char	pBuf[256];
			_snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value less or equal to r2_dof_focus-0.1");
			Msg("~ Invalid syntax in call to '%s'", cName);
			Msg("~ Valid arguments: %s", pBuf);
			Console->Execute("r2_dof_focus");
		}
		else
		{
			CCC_Float::Execute(args);
			if (g_pGamePersistent)
				g_pGamePersistent->SetBaseDof(ps_r2_dof);
		}
	}

	//	CCC_Dof should save all data as well as load from config
	virtual void	Save(IWriter* F) { ; }
};

class CCC_DofFocus : public CCC_Float
{
public:
	CCC_DofFocus(LPCSTR N, float* V, float _min = 0.0f, float _max = 10000.0f)
		: CCC_Float(N, V, _min, _max) {}

	virtual void Execute(LPCSTR args)
	{
		float v = float(atof(args));

		if (v > ps_r2_dof.z - 0.1f)
		{
			char	pBuf[256];
			_snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value less or equal to r2_dof_far-0.1");
			Msg("~ Invalid syntax in call to '%s'", cName);
			Msg("~ Valid arguments: %s", pBuf);
			Console->Execute("r2_dof_far");
		}
		else if (v < ps_r2_dof.x + 0.1f)
		{
			char	pBuf[256];
			_snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value greater or equal to r2_dof_far-0.1");
			Msg("~ Invalid syntax in call to '%s'", cName);
			Msg("~ Valid arguments: %s", pBuf);
			Console->Execute("r2_dof_near");
		}
		else {
			CCC_Float::Execute(args);
			if (g_pGamePersistent)
				g_pGamePersistent->SetBaseDof(ps_r2_dof);
		}
	}

	//	CCC_Dof should save all data as well as load from config
	virtual void	Save(IWriter* F) { ; }
};

class CCC_Dof : public CCC_Vector3
{
public:
	CCC_Dof(LPCSTR N, Fvector* V, const Fvector _min, const Fvector _max) :
		CCC_Vector3(N, V, _min, _max) {
		;
	}

	virtual void	Execute(LPCSTR args)
	{
		Fvector v;
		if (3 != sscanf(args, "%f,%f,%f", &v.x, &v.y, &v.z))
			InvalidSyntax();
		else if ((v.x > v.y - 0.1f) || (v.z < v.y + 0.1f))
		{
			InvalidSyntax();
			Msg("x <= y - 0.1");
			Msg("y <= z - 0.1");
		}
		else
		{
			CCC_Vector3::Execute(args);
			if (g_pGamePersistent)
				g_pGamePersistent->SetBaseDof(ps_r2_dof);
		}
	}
	virtual void	Status(TStatus& S)
	{
		sprintf(S, "%f,%f,%f", value->x, value->y, value->z);
	}
	virtual void	Info(TInfo& I)
	{
		sprintf(I, "vector3 in range [%f,%f,%f]-[%f,%f,%f]", min.x, min.y, min.z, max.x, max.y, max.z);
	}

};

class CCC_detail_radius : public CCC_Integer
{
public:
	void	apply() {
		dm_current_size = iFloor((float)ps_r__detail_radius / 4) * 2;
		dm_current_cache1_line = dm_current_size * 2 / 4;		// assuming cache1_count = 4
		dm_current_cache_line = dm_current_size + 1 + dm_current_size;
		dm_current_cache_size = dm_current_cache_line * dm_current_cache_line;
		dm_current_fade = float(2 * dm_current_size) - .5f;
	}
	CCC_detail_radius(LPCSTR N, int* V, int _min = 0, int _max = 999) : CCC_Integer(N, V, _min, _max) { };
	virtual void Execute(LPCSTR args)
	{
		CCC_Integer::Execute(args);
		apply();
	}
	virtual void	Status(TStatus& S)
	{
		CCC_Integer::Status(S);
	}
};

void		xrRender_initconsole()
{
	CMD3(CCC_Mask, "r2_soft_water", &ps_r2_ls_flags, R2FLAG_SOFT_WATER);
	CMD3(CCC_Mask, "r2_soft_particles", &ps_r2_ls_flags, R2FLAG_SOFT_PARTICLES);
	CMD3(CCC_Mask, "r2_soft_fog", &ps_r2_ls_flags, R2FLAG_SOFT_FOG);

	CMD3(CCC_Token, "r2_aa_type", &ps_aa, aa_token);
	CMD3(CCC_Token, "r2_aa_quality", &ps_aa_quality, aa_quality_token);

	CMD3(CCC_Token, "r2_ao_type", &ps_ao, ao_token);
	CMD3(CCC_Token, "r2_ao_quality", &ps_ao_quality, ao_quality_token);
	CMD3(CCC_Mask,  "r2_ao_blur", &ps_r2_ls_flags_ext, R2FLAGEXT_AO_BLUR);

	CMD3(CCC_Mask, "r2_vignette", &ps_r2_pp_flags, R2FLAG_VIGNETTE);
	CMD3(CCC_Mask, "r2_chromatic_abberation", &ps_r2_pp_flags, R2FLAG_CHROMATIC_ABBERATION);
	CMD3(CCC_Mask, "r2_bloom", &ps_r2_pp_flags, R2FLAG_BLOOM);

	Fvector	tw_min, tw_max;
	tw_min.set(-10000, -10000, 0);	tw_max.set(10000, 10000, 10000);
	CMD4(CCC_Dof,		"r2_dof",			&ps_r2_dof,		tw_min,		tw_max);
	CMD4(CCC_DofNear,	"r2_dof_near",		&ps_r2_dof.x,	tw_min.x,	tw_max.x);
	CMD4(CCC_DofFocus,	"r2_dof_focus",		&ps_r2_dof.y,	tw_min.y,	tw_max.y);
	CMD4(CCC_DofFar,	"r2_dof_far",		&ps_r2_dof.z,	tw_min.z,	tw_max.z);
	CMD4(CCC_Float,		"r2_dof_kernel",	&ps_r2_dof_kernel_size,		.0f, 10.f);
	CMD4(CCC_Float,		"r2_dof_sky",		&ps_r2_dof_sky, -10000.f, 10000.f);
	CMD3(CCC_Mask,		"r2_dof_enabled",	&ps_r2_pp_flags, R2FLAG_DOF);
	CMD3(CCC_Token,		"r2_dof_quality",	&ps_dof_quality, dof_quality_token);

	CMD3(CCC_Mask, "r2_mblur_enabled", &ps_r2_pp_flags, R2FLAG_MBLUR);

	CMD3(CCC_Token, "r2_sun_shafts", &ps_r_sun_shafts, qsun_shafts_token);
	CMD3(CCC_Token, "r2_sun_quality", &ps_sun_quality, sun_quality_token);
	CMD3(CCC_Token, "r2_shadow_filtering", &ps_shadow_filtering, shadow_filter_token);

	CMD3(CCC_Token, "r2_bump_mode", &ps_bump_mode, bump_mode_token);
	CMD3(CCC_Token, "r2_tdetail_bump_mode", &ps_tdetail_bump_mode, tdetail_bump_mode_token);
	CMD3(CCC_Token, "r2_terrain_bump_mode", &ps_terrain_bump_mode, terrain_bump_mode_token);

	CMD3(CCC_Token, "r2_debug_render", &ps_debug_frame_layers, debug_frame_layers_token);
	CMD3(CCC_Token, "r2_debug_textures", &ps_debug_textures, ps_debug_textures_token);

	CMD3(CCC_Mask, "r2_gbuffer_opt", &ps_r2_ls_flags_ext, R2FLAGEXT_GBUFFER_OPT);

	CMD3(CCC_Preset, "_preset", &ps_Preset, qpreset_token);
	CMD3(CCC_EffPreset, "eff_preset", &ps_EffPreset, qeffpreset_token);

	// Common
	CMD1(CCC_Screenshot, "screenshot");
#ifdef DEBUG
#if RENDER==R_R2
	CMD1(CCC_BuildSSA, "build_ssa");
#endif
	CMD4(CCC_Integer, "r__lsleep_frames", &ps_r__LightSleepFrames, 4, 30);
	CMD4(CCC_Float, "r__ssa_glod_start", &ps_r__GLOD_ssa_start, 128, 512);
	CMD4(CCC_Float, "r__ssa_glod_end", &ps_r__GLOD_ssa_end, 16, 96);
	CMD4(CCC_Float, "r__wallmark_shift_pp", &ps_r__WallmarkSHIFT, 0.0f, 1.f);
	CMD4(CCC_Float, "r__wallmark_shift_v", &ps_r__WallmarkSHIFT_V, 0.0f, 1.f);
	CMD1(CCC_ModelPoolStat, "stat_models");
#endif // DEBUG

	//	CMD4(CCC_Integer,	"r__supersample",		&ps_r__Supersample,			1,		4		);
	CMD4(CCC_Float, "r__wallmark_ttl", &ps_r__WallmarkTTL, 1.0f, 5.f * 60.f);

	// KD: extended max from 1.2 to 3.0
	CMD4(CCC_Float, "r__geometry_lod", &ps_r__LOD, 0.1f, 3.0f);
	//.	CMD4(CCC_Float,		"r__geometry_lod_pow",	&ps_r__LOD_Power,			0,		2		);

	//.	CMD4(CCC_Float,		"r__detail_density",	&ps_r__Detail_density,		.05f,	0.99f	);
	// KD: extended min from 0.2 to 0.04 and replaced variable
	CMD4(CCC_Float, "r__detail_density", &ps_current_detail_density, .04f, 0.6f);

#ifdef DEBUG
	CMD4(CCC_Float, "r__detail_l_ambient", &ps_r__Detail_l_ambient, .5f, .95f);
	CMD4(CCC_Float, "r__detail_l_aniso", &ps_r__Detail_l_aniso, .1f, .5f);

	CMD4(CCC_Float, "r__d_tree_w_amp", &ps_r__Tree_w_amp, .001f, 1.f);
	CMD4(CCC_Float, "r__d_tree_w_rot", &ps_r__Tree_w_rot, .01f, 100.f);
	CMD4(CCC_Float, "r__d_tree_w_speed", &ps_r__Tree_w_speed, 1.0f, 10.f);

	tw_min.set(EPS, EPS, EPS);
	tw_max.set(2, 2, 2);
	CMD4(CCC_Vector3, "r__d_tree_wave", &ps_r__Tree_Wave, tw_min, tw_max);
#endif // DEBUG

	CMD2(CCC_tf_Aniso, "r__tf_aniso", &ps_r__tf_Anisotropic); //	{1..16}

	// R1
	CMD4(CCC_Float, "r1_ssa_lod_a", &ps_r1_ssaLOD_A, 16, 96);
	CMD4(CCC_Float, "r1_ssa_lod_b", &ps_r1_ssaLOD_B, 16, 64);
	CMD4(CCC_Float, "r1_lmodel_lerp", &ps_r1_lmodel_lerp, 0, 0.333f);
	CMD2(CCC_tf_MipBias, "r1_tf_mipbias", &ps_r1_tf_Mipbias);//	{-3 +3}
	CMD3(CCC_Mask, "r1_dlights", &ps_r1_flags, R1FLAG_DLIGHTS);
	CMD4(CCC_Float, "r1_dlights_clip", &ps_r1_dlights_clip, 10.f, 150.f);
	CMD4(CCC_Float, "r1_pps_u", &ps_r1_pps_u, -1.f, +1.f);
	CMD4(CCC_Float, "r1_pps_v", &ps_r1_pps_v, -1.f, +1.f);
	CMD4(CCC_Float, "r1_dlights_clip", &ps_r1_dlights_clip, 10.f, 150.f);

	// R1-specific
	CMD4(CCC_Integer, "r1_glows_per_frame", &ps_r1_GlowsPerFrame, 2, 32);

	// R2
	CMD4(CCC_Float, "r2_ssa_lod_a", &ps_r2_ssaLOD_A, 16, 96);
	CMD4(CCC_Float, "r2_ssa_lod_b", &ps_r2_ssaLOD_B, 32, 64);
	CMD2(CCC_tf_MipBias, "r2_tf_mipbias", &ps_r2_tf_Mipbias);

	// R2-specific
	CMD2(CCC_R2GM, "r2em", &ps_r2_gmaterial);
	CMD3(CCC_Mask, "r2_tonemap", &ps_r2_ls_flags, R2FLAG_TONEMAP);
	CMD4(CCC_Float, "r2_tonemap_middlegray", &ps_r2_tonemap_middlegray, 0.0f, 2.0f);
	CMD4(CCC_Float, "r2_tonemap_adaptation", &ps_r2_tonemap_adaptation, 0.01f, 10.0f);
	CMD4(CCC_Float, "r2_tonemap_lowlum", &ps_r2_tonemap_low_lum, 0.0001f, 1.0f);
	CMD4(CCC_Float, "r2_tonemap_amount", &ps_r2_tonemap_amount, 0.0000f, 1.0f);
	CMD4(CCC_Float, "r2_ls_bloom_kernel_scale", &ps_r2_ls_bloom_kernel_scale, 0.5f, 2.f);
	CMD4(CCC_Float, "r2_ls_bloom_kernel_g", &ps_r2_ls_bloom_kernel_g, 1.f, 7.f);
	CMD4(CCC_Float, "r2_ls_bloom_kernel_b", &ps_r2_ls_bloom_kernel_b, 0.01f, 1.f);
	CMD4(CCC_Float, "r2_ls_bloom_threshold", &ps_r2_ls_bloom_threshold, 0.f, 1.f);
	CMD4(CCC_Float, "r2_ls_bloom_speed", &ps_r2_ls_bloom_speed, 0.f, 100.f);
	CMD3(CCC_Mask, "r2_ls_bloom_fast", &ps_r2_ls_flags, R2FLAG_FASTBLOOM);
	CMD4(CCC_Float, "r2_ls_dsm_kernel", &ps_r2_ls_dsm_kernel, .1f, 3.f);
	CMD4(CCC_Float, "r2_ls_psm_kernel", &ps_r2_ls_psm_kernel, .1f, 3.f);
	CMD4(CCC_Float, "r2_ls_ssm_kernel", &ps_r2_ls_ssm_kernel, .1f, 3.f);
	CMD4(CCC_Float, "r2_ls_squality", &ps_r2_ls_squality, .5f, 1.f);

	CMD3(CCC_Mask, "r2_zfill", &ps_r2_ls_flags, R2FLAG_ZFILL);
	CMD4(CCC_Float, "r2_zfill_depth", &ps_r2_zfill, .001f, .5f);
	CMD3(CCC_Mask, "r2_allow_r1_lights", &ps_r2_ls_flags, R2FLAG_R1LIGHTS);

	//- Mad Max
	CMD4(CCC_Float, "r2_gloss_factor", &ps_r2_gloss_factor, 1.f, 3.f);
	//- Mad Max

//#ifdef DEBUG
	CMD3(CCC_Mask, "r2_use_nvdbt", &ps_r2_ls_flags, R2FLAG_USE_NVDBT);
	CMD3(CCC_Mask, "r2_mt", &ps_r2_ls_flags, R2FLAG_EXP_MT_CALC);
//#endif // DEBUG

	CMD3(CCC_Mask, "r2_sun", &ps_r2_ls_flags, R2FLAG_SUN);
	CMD3(CCC_Mask, "r2_sun_details", &ps_r2_ls_flags, R2FLAG_SUN_DETAILS);
	CMD3(CCC_Mask, "r2_sun_focus", &ps_r2_ls_flags, R2FLAG_SUN_FOCUS);
	//	CMD3(CCC_Mask,		"r2_sun_static",		&ps_r2_ls_flags,			R2FLAG_SUN_STATIC);
	//	CMD3(CCC_Mask,		"r2_exp_splitscene",	&ps_r2_ls_flags,			R2FLAG_EXP_SPLIT_SCENE);
	//	CMD3(CCC_Mask,		"r2_exp_donttest_uns",	&ps_r2_ls_flags,			R2FLAG_EXP_DONT_TEST_UNSHADOWED);

	CMD3(CCC_Mask, "r2_sun_tsm", &ps_r2_ls_flags, R2FLAG_SUN_TSM);
	CMD4(CCC_Float, "r2_sun_tsm_proj", &ps_r2_sun_tsm_projection, .001f, 0.8f);
	CMD4(CCC_Float, "r2_sun_tsm_bias", &ps_r2_sun_tsm_bias, -0.5, +0.5);
	CMD4(CCC_Float, "r2_sun_near", &ps_r2_sun_near, 1.f, 50.f);
	CMD4(CCC_Float, "r2_sun_far", &ps_r2_sun_far, 100.f, 360.f);
	CMD4(CCC_Float, "r2_sun_near_border", &ps_r2_sun_near_border, .5f, 1.0f);
	CMD4(CCC_Float, "r2_sun_depth_far_scale", &ps_r2_sun_depth_far_scale, 0.5, 1.5);
	CMD4(CCC_Float, "r2_sun_depth_far_bias", &ps_r2_sun_depth_far_bias, -0.5, +0.5);
	CMD4(CCC_Float, "r2_sun_depth_near_scale", &ps_r2_sun_depth_near_scale, 0.5, 1.5);
	CMD4(CCC_Float, "r2_sun_depth_near_bias", &ps_r2_sun_depth_near_bias, -0.5, +0.5);
	CMD4(CCC_Float, "r2_sun_lumscale", &ps_r2_sun_lumscale, -1.0, +3.0);
	CMD4(CCC_Float, "r2_sun_lumscale_hemi", &ps_r2_sun_lumscale_hemi, 0.0, +3.0);
	CMD4(CCC_Float, "r2_sun_lumscale_amb", &ps_r2_sun_lumscale_amb, 0.0, +3.0);

	CMD3(CCC_Mask, "r2_aa", &ps_r2_ls_flags, R2FLAG_AA);
	CMD4(CCC_Float, "r2_aa_kernel", &ps_r2_aa_kernel, 0.3f, 0.7f);
	CMD4(CCC_Float, "r2_mblur", &ps_r2_mblur, 0.0f, 1.0f);

	CMD3(CCC_Mask, "r2_gi", &ps_r2_ls_flags, R2FLAG_GI);
	CMD4(CCC_Float, "r2_gi_clip", &ps_r2_GI_clip, EPS, 0.1f);
	CMD4(CCC_Integer, "r2_gi_depth", &ps_r2_GI_depth, 1, 5);
	CMD4(CCC_Integer, "r2_gi_photons", &ps_r2_GI_photons, 8, 256);
	CMD4(CCC_Float, "r2_gi_refl", &ps_r2_GI_refl, EPS_L, 0.99f);

	CMD4(CCC_Integer, "r2_wait_sleep", &ps_r2_wait_sleep, 0, 1);

#ifdef DEBUG
	CMD4(CCC_Integer, "r2_dhemi_count", &ps_r2_dhemi_count, 4, 25);
	CMD4(CCC_Float, "r2_dhemi_scale", &ps_r2_dhemi_scale, .5f, 3.f);
	CMD4(CCC_Float, "r2_dhemi_smooth", &ps_r2_lt_smooth, 0.f, 10.f);
#endif // DEBUG

	CMD3(CCC_Mask, "r2_shadow_cascede_zcul", &ps_r2_ls_flags_ext, R2FLAGEXT_SUN_ZCULLING);
	CMD3(CCC_Mask, "r2_shadow_cascede_old", &ps_r2_ls_flags_ext, R2FLAGEXT_SUN_OLD);

	CMD4(CCC_Float, "r2_ls_depth_scale", &ps_r2_ls_depth_scale, 0.5, 1.5);
	CMD4(CCC_Float, "r2_ls_depth_bias", &ps_r2_ls_depth_bias, -0.5, +0.5);

	CMD4(CCC_Float, "r2_parallax_h", &ps_r2_df_parallax_h, .0f, .5f);
	//	CMD4(CCC_Float,		"r2_parallax_range",	&ps_r2_df_parallax_range,	5.0f,	175.0f	);

	CMD4(CCC_Float, "r2_slight_fade", &ps_r2_slight_fade, .02f, 2.f);

	tw_min.set(0, 0, 0);	tw_max.set(1, 1, 1);
	CMD4(CCC_Vector3, "r2_aa_break", &ps_r2_aa_barier, tw_min, tw_max);

	tw_min.set(0, 0, 0);	tw_max.set(1, 1, 1);
	CMD4(CCC_Vector3, "r2_aa_weight", &ps_r2_aa_weight, tw_min, tw_max);

	CMD3(CCC_Mask, "r__bloodmarks", &ps_common_flags, RFLAG_BLOODMARKS);
	CMD4(CCC_detail_radius, "r__detail_radius", &ps_r__detail_radius, 49, 250);
	CMD4(CCC_Float, "r__detail_scale", &ps_current_detail_scale, 0.2f, 3.0f);
	CMD3(CCC_Mask, "r__actor_shadow", &ps_common_flags, RFLAG_ACTOR_SHADOW);

}

void	xrRender_apply_tf()
{
	Console->Execute("r__tf_aniso");
#if RENDER==R_R1
	Console->Execute("r1_tf_mipbias");
#else
	Console->Execute("r2_tf_mipbias");
#endif
}

#endif
