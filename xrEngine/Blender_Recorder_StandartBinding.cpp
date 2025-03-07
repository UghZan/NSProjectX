#include "stdafx.h"
#pragma hdrstop

#pragma warning(push)
#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(pop)

#include "ResourceManager.h"
#include "blenders\Blender_Recorder.h"
#include "blenders\Blender.h"

#include "igame_persistent.h"
#include "environment.h"

// matrices
#define	BIND_DECLARE(xf)	\
class cl_xform_##xf	: public R_constant_setup {	virtual void setup (R_constant* C) { RCache.xforms.set_c_##xf (C); } }; \
	static cl_xform_##xf	binder_##xf
BIND_DECLARE(w);
BIND_DECLARE(invw);
BIND_DECLARE(v);
BIND_DECLARE(p);
BIND_DECLARE(wv);
BIND_DECLARE(vp);
BIND_DECLARE(wvp);

#define DECLARE_TREE_BIND(c)	\
	class cl_tree_##c: public R_constant_setup	{virtual void setup(R_constant* C) {RCache.tree.set_c_##c(C);} };	\
	static cl_tree_##c	tree_binder_##c

DECLARE_TREE_BIND(m_xform_v);
DECLARE_TREE_BIND(m_xform);
DECLARE_TREE_BIND(consts);
DECLARE_TREE_BIND(wave);
DECLARE_TREE_BIND(wind);
DECLARE_TREE_BIND(c_scale);
DECLARE_TREE_BIND(c_bias);
DECLARE_TREE_BIND(c_sun);

class cl_hemi_cube_pos_faces : public R_constant_setup
{
	virtual void setup(R_constant* C) { RCache.hemi.set_c_pos_faces(C); }
};

static cl_hemi_cube_pos_faces binder_hemi_cube_pos_faces;

class cl_hemi_cube_neg_faces : public R_constant_setup
{
	virtual void setup(R_constant* C) { RCache.hemi.set_c_neg_faces(C); }
};

static cl_hemi_cube_neg_faces binder_hemi_cube_neg_faces;

class cl_material : public R_constant_setup
{
	virtual void setup(R_constant* C) { RCache.hemi.set_c_material(C); }
};

static cl_material binder_material;

class cl_texgen : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		Fmatrix mTexgen;

		float	_w = float(Device.dwWidth);
		float	_h = float(Device.dwHeight);
		float	o_w = (.5f / _w);
		float	o_h = (.5f / _h);
		Fmatrix			mTexelAdjust =
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				1.0f,			0.0f,
			0.5f + o_w,			0.5f + o_h,			0.0f,			1.0f
		};

		mTexgen.mul(mTexelAdjust, RCache.xforms.m_wvp);

		RCache.set_c(C, mTexgen);
	}
};
static cl_texgen		binder_texgen;

class cl_VPtexgen : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		Fmatrix mTexgen;

		float	_w = float(Device.dwWidth);
		float	_h = float(Device.dwHeight);
		float	o_w = (.5f / _w);
		float	o_h = (.5f / _h);
		Fmatrix			mTexelAdjust =
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				1.0f,			0.0f,
			0.5f + o_w,			0.5f + o_h,			0.0f,			1.0f
		};

		mTexgen.mul(mTexelAdjust, RCache.xforms.m_vp);

		RCache.set_c(C, mTexgen);
	}
};
static cl_VPtexgen		binder_VPtexgen;

// fog
class cl_fog_plane : public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C)
	{
		if (marker != Device.dwFrame)
		{
			// Plane
			Fvector4		plane;
			Fmatrix& M = Device.mFullTransform;
			plane.x = -(M._14 + M._13);
			plane.y = -(M._24 + M._23);
			plane.z = -(M._34 + M._33);
			plane.w = -(M._44 + M._43);
			float denom = -1.0f / _sqrt(_sqr(plane.x) + _sqr(plane.y) + _sqr(plane.z));
			plane.mul(denom);

			// Near/Far
			float A = g_pGamePersistent->Environment().CurrentEnv->fog_near;
			float B = 1 / (g_pGamePersistent->Environment().CurrentEnv->fog_far - A);

			result.set(-plane.x * B, -plane.y * B, -plane.z * B, 1 - (plane.w - A) * B);								// view-plane
		}
		RCache.set_c(C, result);
	}
};
static cl_fog_plane		binder_fog_plane;

// fog-params
class cl_fog_params : public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C)
	{
		if (marker != Device.dwFrame)
		{
			// Near/Far
			float	n = g_pGamePersistent->Environment().CurrentEnv->fog_near;
			float	f = g_pGamePersistent->Environment().CurrentEnv->fog_far;
			float	r = 1 / (f - n);
			result.set(-n * r, n, f, r);
		}
		RCache.set_c(C, result);
	}
};	static cl_fog_params	binder_fog_params;

// fog-color
class cl_fog_color : public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C) {
		if (marker != Device.dwFrame) {
			CEnvDescriptor* desc = g_pGamePersistent->Environment().CurrentEnv;
			result.set(desc->fog_color.x, desc->fog_color.y, desc->fog_color.z, 0);
		}
		RCache.set_c(C, result);
	}
};	static cl_fog_color		binder_fog_color;

// times
class cl_times : public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		float 		t = Device.fTimeGlobal;
		RCache.set_c(C, t, t * 10, t / 10, _sin(t));
	}
};
static cl_times		binder_times;

// eye-params
class cl_eye_P : public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		Fvector& V = Device.vCameraPosition;
		RCache.set_c(C, V.x, V.y, V.z, 1);
	}
};
static cl_eye_P		binder_eye_P;

// eye-params
class cl_eye_D : public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		Fvector& V = Device.vCameraDirection;
		RCache.set_c(C, V.x, V.y, V.z, 0);
	}
};
static cl_eye_D		binder_eye_D;

// eye-params
class cl_eye_N : public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		Fvector& V = Device.vCameraTop;
		RCache.set_c(C, V.x, V.y, V.z, 0);
	}
};
static cl_eye_N		binder_eye_N;

// D-Light0
class cl_sun0_color : public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C) {
		if (marker != Device.dwFrame) {
			CEnvDescriptor* desc = g_pGamePersistent->Environment().CurrentEnv;
			result.set(desc->sun_color.x, desc->sun_color.y, desc->sun_color.z, 0);
		}
		RCache.set_c(C, result);
	}
};	static cl_sun0_color		binder_sun0_color;

static class cl_env_color : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		CEnvDescriptorMixer* envdesc = g_pGamePersistent->Environment().CurrentEnv;
		Fvector4 envclr = { envdesc->hemi_color.x * 2 + EPS,	envdesc->hemi_color.y * 2 + EPS,	envdesc->hemi_color.z * 2 + EPS,	envdesc->weight };
		RCache.set_c(C, envclr);
	}
}	binder_env_color;

class cl_sun0_dir_w : public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C) {
		if (marker != Device.dwFrame) {
			CEnvDescriptor* desc = g_pGamePersistent->Environment().CurrentEnv;
			result.set(desc->sun_dir.x, desc->sun_dir.y, desc->sun_dir.z, 0);
		}
		RCache.set_c(C, result);
	}
};	static cl_sun0_dir_w		binder_sun0_dir_w;
class cl_sun0_dir_e : public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C) {
		if (marker != Device.dwFrame) {
			Fvector D;
			CEnvDescriptor* desc = g_pGamePersistent->Environment().CurrentEnv;
			Device.mView.transform_dir(D, desc->sun_dir);
			D.normalize();
			result.set(D.x, D.y, D.z, 0);
		}
		RCache.set_c(C, result);
	}
};	static cl_sun0_dir_e		binder_sun0_dir_e;

//
class cl_amb_color : public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C) {
		if (marker != Device.dwFrame) {
			CEnvDescriptorMixer* desc = g_pGamePersistent->Environment().CurrentEnv;
			result.set(desc->ambient.x, desc->ambient.y, desc->ambient.z, desc->weight);
		}
		RCache.set_c(C, result);
	}
};	static cl_amb_color		binder_amb_color;
class cl_hemi_color : public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C) {
		if (marker != Device.dwFrame) {
			CEnvDescriptor* desc = g_pGamePersistent->Environment().CurrentEnv;
			result.set(desc->hemi_color.x, desc->hemi_color.y, desc->hemi_color.z, desc->hemi_color.w);
		}
		RCache.set_c(C, result);
	}
};	static cl_hemi_color		binder_hemi_color;

static class cl_screen_res : public R_constant_setup
{
	virtual void setup(R_constant* C)
	{
		RCache.set_c(C, (float)Device.dwWidth, (float)Device.dwHeight, 1.0f / (float)Device.dwWidth, 1.0f / (float)Device.dwHeight);
	}
}	binder_screen_res;

static class cl_screen_params : public R_constant_setup
{
	u32 marker;
	Fvector4 result;
	void setup(R_constant* C) override
	{
		if (marker != Device.dwFrame) {
			result.set(Device.fFOV, Device.fASPECT, tan(deg2rad(Device.fFOV) / 2), g_pGamePersistent->Environment().CurrentEnv->far_plane * 0.75f);
		}
		RCache.set_c(C, result);
	}
} binder_screen_params;

// Standart constant-binding
void	CBlender_Compile::SetMapping()
{
	// matrices
	r_Constant("m_W", &binder_w);
	r_Constant("m_invW", &binder_invw);
	r_Constant("m_V", &binder_v);
	r_Constant("m_P", &binder_p);
	r_Constant("m_WV", &binder_wv);
	r_Constant("m_VP", &binder_vp);
	r_Constant("m_WVP", &binder_wvp);

	r_Constant("m_xform_v", &tree_binder_m_xform_v);
	r_Constant("m_xform", &tree_binder_m_xform);
	r_Constant("consts", &tree_binder_consts);
	r_Constant("wave", &tree_binder_wave);
	r_Constant("wind", &tree_binder_wind);
	r_Constant("c_scale", &tree_binder_c_scale);
	r_Constant("c_bias", &tree_binder_c_bias);
	r_Constant("c_sun", &tree_binder_c_sun);

	//hemi cube
	r_Constant("L_material", &binder_material);
	r_Constant("hemi_cube_pos_faces", &binder_hemi_cube_pos_faces);
	r_Constant("hemi_cube_neg_faces", &binder_hemi_cube_neg_faces);

	//	Igor	temp solution for the texgen functionality in the shader
	r_Constant("m_texgen", &binder_texgen);
	r_Constant("mVPTexgen", &binder_VPtexgen);

#ifndef _EDITOR
	// fog-params
	r_Constant("fog_plane", &binder_fog_plane);
	r_Constant("fog_params", &binder_fog_params);
	r_Constant("fog_color", &binder_fog_color);
#endif
	// env-params
	r_Constant("env_color", &binder_env_color);

	// time
	r_Constant("timers", &binder_times);

	// eye-params
	r_Constant("eye_position", &binder_eye_P);
	r_Constant("eye_direction", &binder_eye_D);
	r_Constant("eye_normal", &binder_eye_N);

#ifndef _EDITOR
	// global-lighting (env params)
	r_Constant("L_sun_color", &binder_sun0_color);
	r_Constant("L_sun_dir_w", &binder_sun0_dir_w);
	r_Constant("L_sun_dir_e", &binder_sun0_dir_e);
	//	r_Constant				("L_lmap_color",	&binder_lm_color);
	r_Constant("L_hemi_color", &binder_hemi_color);
	r_Constant("L_ambient", &binder_amb_color);
#endif

	r_Constant("screen_res", &binder_screen_res);
	r_Constant("ogse_c_screen", &binder_screen_params);

	// detail
	//if (bDetail	&& detail_scaler)
	//	Igor: bDetail can be overridden by no_detail_texture option.
	//	But shader can be deatiled implicitly, so try to set this parameter
	//	anyway.
	if (detail_scaler)
		r_Constant("dt_params", detail_scaler);

	// other common
	for (u32 it = 0; it < Device.Resources->v_constant_setup.size(); it++)
	{
		std::pair<shared_str, R_constant_setup*>	cs = Device.Resources->v_constant_setup[it];
		r_Constant(*cs.first, cs.second);
	}
}