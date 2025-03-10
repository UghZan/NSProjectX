#ifndef xrRender_consoleH
#define xrRender_consoleH
#pragma once

// Common
extern ENGINE_API	BOOL		r2_advanced_pp;
extern ENGINE_API	int			ps_r__Supersample;
extern ECORE_API	int			ps_r__LightSleepFrames;

extern ECORE_API	u32			ps_r_sun_shafts;	//=	0;
extern ECORE_API	xr_token	qsun_shafts_token[];

extern ECORE_API	float		ps_r__Detail_l_ambient;
extern ECORE_API	float		ps_r__Detail_l_aniso;
extern ECORE_API	float		ps_r__Detail_density;

extern ECORE_API	float		ps_r__Tree_w_rot;
extern ECORE_API	float		ps_r__Tree_w_speed;
extern ECORE_API	float		ps_r__Tree_w_amp;
extern ECORE_API	float		ps_r__Tree_SBC;		// scale bias correct
extern ECORE_API	Fvector		ps_r__Tree_Wave;

extern ECORE_API	float		ps_r__WallmarkTTL		;
extern ECORE_API	float		ps_r__WallmarkSHIFT		;
extern ECORE_API	float		ps_r__WallmarkSHIFT_V	;

extern ECORE_API	float		ps_r__GLOD_ssa_start;
extern ECORE_API	float		ps_r__GLOD_ssa_end	;
extern ECORE_API	float		ps_r__LOD			;
//.extern ECORE_API	float		ps_r__LOD_Power		;
extern ECORE_API	float		ps_r__ssaDISCARD	;
extern ECORE_API	float		ps_r__ssaDONTSORT	;
extern ECORE_API	float		ps_r__ssaHZBvsTEX	;
extern ECORE_API	int			ps_r__tf_Anisotropic;

// R1
extern ECORE_API	float		ps_r1_ssaLOD_A;
extern ECORE_API	float		ps_r1_ssaLOD_B;
extern ECORE_API	float		ps_r1_tf_Mipbias;
extern ECORE_API	float		ps_r1_lmodel_lerp;
extern ECORE_API	float		ps_r1_dlights_clip;
extern ECORE_API	float		ps_r1_pps_u;
extern ECORE_API	float		ps_r1_pps_v;

// R1-specific
extern ECORE_API	int			ps_r1_GlowsPerFrame;	// r1-only
extern ECORE_API	Flags32		ps_r1_flags;			// r1-only
enum
{
	R1FLAG_DLIGHTS				= (1<<0),
};

// R2
extern ECORE_API	float		ps_r2_ssaLOD_A;
extern ECORE_API	float		ps_r2_ssaLOD_B;
extern ECORE_API	float		ps_r2_tf_Mipbias;

// R2-specific
extern ECORE_API Flags32		ps_r2_ls_flags;				// r2-only
extern ECORE_API Flags32		ps_r2_pp_flags;				// r2.5-only
extern ECORE_API Flags32		ps_r2_ls_flags_ext;				// r2-only
extern ECORE_API float			ps_r2_df_parallax_h;		// r2-only
extern ECORE_API float			ps_r2_df_parallax_range;	// r2-only
extern ECORE_API float			ps_r2_gmaterial;			// r2-only
extern ECORE_API float			ps_r2_tonemap_middlegray;	// r2-only
extern ECORE_API float			ps_r2_tonemap_adaptation;	// r2-only
extern ECORE_API float			ps_r2_tonemap_low_lum;		// r2-only
extern ECORE_API float			ps_r2_tonemap_amount;		// r2-only
extern ECORE_API float			ps_r2_ls_bloom_kernel_scale;// r2-only	// gauss
extern ECORE_API float			ps_r2_ls_bloom_kernel_g;	// r2-only	// gauss
extern ECORE_API float			ps_r2_ls_bloom_kernel_b;	// r2-only	// bilinear
extern ECORE_API float			ps_r2_ls_bloom_threshold;	// r2-only
extern ECORE_API float			ps_r2_ls_bloom_speed;		// r2-only
extern ECORE_API float			ps_r2_ls_dsm_kernel;		// r2-only
extern ECORE_API float			ps_r2_ls_psm_kernel;		// r2-only
extern ECORE_API float			ps_r2_ls_ssm_kernel;		// r2-only
extern ECORE_API Fvector		ps_r2_aa_barier;			// r2-only
extern ECORE_API Fvector		ps_r2_aa_weight;			// r2-only
extern ECORE_API float			ps_r2_aa_kernel;			// r2-only
extern ECORE_API float			ps_r2_mblur;				// .5f
extern ECORE_API int			ps_r2_GI_depth;				// 1..5
extern ECORE_API int			ps_r2_GI_photons;			// 8..256
extern ECORE_API float			ps_r2_GI_clip;				// EPS
extern ECORE_API float			ps_r2_GI_refl;				// .9f
extern ECORE_API float			ps_r2_ls_depth_scale;		// 1.0f
extern ECORE_API float			ps_r2_ls_depth_bias;		// -0.0001f
extern ECORE_API float			ps_r2_ls_squality;			// 1.0f
extern ECORE_API float			ps_r2_sun_near;				// 10.0f
extern ECORE_API float			ps_r2_sun_far;
extern ECORE_API u32			ps_sun_quality;
extern ECORE_API float			ps_r2_sun_near_border;		// 1.0f
extern ECORE_API float			ps_r2_sun_tsm_projection;	// 0.2f
extern ECORE_API float			ps_r2_sun_tsm_bias;			// 0.0001f
extern ECORE_API float			ps_r2_sun_depth_far_scale;	// 1.00001f
extern ECORE_API float			ps_r2_sun_depth_far_bias;	// -0.0001f
extern ECORE_API float			ps_r2_sun_depth_near_scale;	// 1.00001f
extern ECORE_API float			ps_r2_sun_depth_near_bias;	// -0.0001f
extern ECORE_API float			ps_r2_sun_lumscale;			// 0.5f
extern ECORE_API float			ps_r2_sun_lumscale_hemi;	// 1.0f
extern ECORE_API float			ps_r2_sun_lumscale_amb;		// 1.0f
extern ECORE_API float			ps_r2_zfill;				// .1f

extern ECORE_API float			ps_r2_dhemi_scale;			// 1.5f
extern ECORE_API int			ps_r2_dhemi_count;			// 5
extern ECORE_API float			ps_r2_slight_fade;			// 1.f
extern ECORE_API int			ps_r2_wait_sleep;

extern ECORE_API u32			ps_aa;
extern ECORE_API u32			ps_aa_quality;

extern ECORE_API u32			ps_ao;
extern ECORE_API u32			ps_ao_quality;

extern ECORE_API u32			ps_debug_frame_layers;

extern ECORE_API u32			ps_sun_quality;
extern ECORE_API u32			ps_shadow_filtering;

extern ECORE_API u32			ps_blur_type;
extern ECORE_API u32			ps_dof_quality;

extern ECORE_API u32			ps_bump_mode;
extern ECORE_API u32			ps_tdetail_bump_mode;
extern ECORE_API u32			ps_terrain_bump_mode;

//	x - min (0), y - focus (1.4), z - max (100)
extern ECORE_API Fvector3		ps_r2_dof;
extern ECORE_API float			ps_r2_dof_sky;				//	distance to sky
extern ECORE_API float			ps_r2_dof_kernel_size;		//	7.0f

extern ECORE_API Flags32		ps_common_flags;				// r1+r2
extern ECORE_API int			ps_r__detail_radius;

enum
{
	R2FLAG_SUN								= (1<<0),
	R2FLAG_SUN_FOCUS						= (1<<1),
	R2FLAG_SUN_TSM							= (1<<2),
	R2FLAG_SUN_DETAILS						= (1<<3),
	R2FLAG_TONEMAP							= (1<<4),
	R2FLAG_AA								= (1<<5),
	R2FLAG_GI								= (1<<6),
	R2FLAG_FASTBLOOM						= (1<<7),
	R2FLAG_GLOBALMATERIAL					= (1<<8),
	R2FLAG_ZFILL							= (1<<9),
	R2FLAG_R1LIGHTS							= (1<<10),
	R2FLAG_SUN_IGNORE_PORTALS				= (1<<11),
	
	R2FLAG_EXP_SPLIT_SCENE					= (1<<12),
	R2FLAG_EXP_DONT_TEST_UNSHADOWED			= (1<<13),

	R2FLAG_USE_NVDBT						= (1<<14),
	R2FLAG_USE_NVSTENCIL					= (1<<15),

	R2FLAG_EXP_MT_CALC						= (1<<16),

	R2FLAG_SOFT_WATER						= (1 << 17),
	R2FLAG_SOFT_PARTICLES					= (1 << 18),
	R2FLAG_SOFT_FOG							= (1 << 19),
	R2FLAG_VIGNETTE							= (1 << 20),
	R2FLAG_CHROMATIC_ABBERATION				= (1 << 21),
	R2FLAG_BLOOM							= (1 << 22),
	R2FLAG_DOF								= (1 << 23),
	R2FLAG_MBLUR							= (1 << 24)

};

enum
{
	R2FLAGEXT_SUN_ZCULLING					= (1 << 0),
	R2FLAGEXT_SUN_OLD						= (1 << 1),
	R2FLAGEXT_AO_BLUR						= (1 << 2),
	R2FLAGEXT_GBUFFER_OPT					= (1 << 3),
};

extern void						xrRender_initconsole	();
extern BOOL						xrRender_test_hw		();
extern void						xrRender_apply_tf		();

enum
{
	RFLAG_BLOODMARKS = (1 << 0),
	RFLAG_ACTOR_SHADOW = (1 << 1)
};

#endif
