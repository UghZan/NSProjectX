#include "stdafx.h"
#pragma hdrstop

#include "Blender_combine.h"

CBlender_combine::CBlender_combine	()	{	description.CLS		= 0;	}
CBlender_combine::~CBlender_combine	()	{	}
 
void	CBlender_combine::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// combine
		C.r_Pass			("combine_1",		"combine_1",		FALSE,	FALSE,	FALSE, TRUE, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA);	//. MRT-blend?
		C.r_Sampler_rtf		("s_position",		r2_RT_P				);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N				);
		C.r_Sampler_rtf		("s_diffuse",		r2_RT_albedo		);
		C.r_Sampler_rtf		("s_accumulator",	r2_RT_accum			);
		C.r_Sampler_rtf		("s_depth",			r2_RT_depth			);
		C.r_Sampler_rtf		("s_tonemap",		r2_RT_luminance_cur	);
		C.r_Sampler_clw		("s_material",		r2_material			);
		C.r_Sampler_clf		("s_ao",			r2_RT_ao1			);
		C.r_Sampler_clf		("s_ao_blurred",	r2_RT_ao2			);
		C.r_Sampler_clf		("s_ao_blurred1",	r2_RT_ao3			);
		C.r_Sampler_clf		("s_ao_blurred2",	r2_RT_ao4			);
		C.r_Sampler_clf		("s_position_blurred",	r2_RT_blurred_position);
		C.r_Sampler_clf		("env_s0",			r2_T_envs0			);
		C.r_Sampler_clf		("env_s1",			r2_T_envs1			);
		C.r_Sampler_clf		("sky_s0",			r2_T_sky0			);
		C.r_Sampler_clf		("sky_s1",			r2_T_sky1			);
		C.r_Sampler_rtf		("s_vollight",		r2_RT_generic2		);
		C.r_Sampler_tex		("s_blue_noise",	"noise\\blue_noise_texture");
		C.r_Sampler_tex		("s_debug_mask",	"ed\\debug_mask");
		C.r_Sampler_tex		("s_debug_diagonal_mask",	"ed\\debug_diagonal_mask");
		C.r_Sampler_tex		("s_debug_triple_mask",		"ed\\debug_triple_mask");

		if (RImplementation.o.HW_smap) {
			if (RImplementation.o.HW_smap_PCF)	C.r_Sampler_clf("s_smap", r2_RT_smap_depth);
			else {
				C.r_Sampler_rtf("s_smap", r2_RT_smap_depth);
			}
		}
		else
			C.r_Sampler_rtf("s_smap", r2_RT_smap_surf);

		jitter(C);
		C.r_End				();
		break;
	case 1:	// aa-edge-detection + AA :)
		C.r_Pass			("null",			"combine_2_AA",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
		C.r_End				();
		break;
	case 2:	// non-AA
		C.r_Pass			("null",			"combine_2_NAA",	FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
		C.r_End				();
		break;
	case 3:	// aa-edge-detection + AA :) + DISTORTION
		C.r_Pass			("null",			"combine_2_AA_D",	FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
		C.r_End				();
		break;
	case 4:	// non-AA + DISTORTION
		C.r_Pass			("null",			"combine_2_NAA_D",	FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
		C.r_End				();
		break;
	case 5:	// post-processing
		C.r_Pass			("null",			"combine_3_PP",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_image",			r2_RT_albedo);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_tex		("s_vignette",		"vfx\\vfx_vignette");
		C.r_End();
		break;
	}
}
