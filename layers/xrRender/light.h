#pragma once

#include "..\xrEngine\ispatial.h"
#if RENDER==R_R2
	#include "light_package.h"
	#include "light_smapvis.h"
	#include "light_GI.h"
#endif

#define MIN_VIRTUAL_SIZE 0.01f

class	light		:	public IRender_Light, public ISpatial
{
public:

	virtual float							get_cone() { return cone; }
	virtual Fcolor							get_color() { return color; }
	virtual float							get_range() { return range; }
	virtual float							get_virtual_size()
	{
#if RENDER==R_R2
		return virtual_size;
#else
		return 0.0;
#endif
	}

	struct {
		u32			type	:	4;
		u32			bStatic	:	1;
		u32			bActive	:	1;
		u32			bShadow	:	1;
	}				flags;
	Fvector			position	;
	Fvector			direction	;
	Fvector			right		;
	float			range		;
	float			cone		;
	Fcolor			color		;

	vis_data		hom			;
	u32				frame_render;

#if RENDER==R_R2
	float						virtual_size;
	light*						omnipart	[6]	;
	xr_vector<light_indirect>	indirect		;
	u32							indirect_photons;

	smapvis			svis;		// used for 6-cubemap faces

	ref_shader		s_spot;
	ref_shader		s_point;

	u32				m_xform_frame;
	Fmatrix			m_xform;

	struct _vis		{
		u32			frame2test;		// frame the test is sheduled to
		u32			query_id;		// ID of occlusion query
		u32			query_order;	// order of occlusion query
		bool		visible;		// visible/invisible
		bool		pending;		// test is still pending
		u16			smap_ID;
	}				vis;

	union			_xform	{
		struct		_D		{
			Fmatrix						combine	;
			s32							minX,maxX	;
			s32							minY,maxY	;
			BOOL						transluent	;
		}	D;
		struct		_P		{
			Fmatrix						world		;
			Fmatrix						view		;
			Fmatrix						project		;
			Fmatrix						combine		;
		}	P;
		struct		_S		{
			Fmatrix						view		;
			Fmatrix						project		;
			Fmatrix						combine		;
			u32							size		;
			u32							posX		;
			u32							posY		;
			BOOL						transluent	;
		}	S;
	}	X;
#endif

public:
	virtual void	set_type				(LT type)						{ flags.type = type;		}
	virtual void	set_active				(bool b);
	virtual bool	get_active				()								{ return flags.bActive;		}
	virtual void	set_shadow				(bool b)						
	{ 
		flags.bShadow=b;			
	}
	virtual void	set_position			(const Fvector& P);
	virtual void	set_rotation			(const Fvector& D, const Fvector& R);
	virtual void	set_cone				(float angle);
	virtual void	set_range				(float R);
	virtual void	set_virtual_size		(float R)						{
#if RENDER==R_R2
		virtual_size = (R > MIN_VIRTUAL_SIZE) ? R : MIN_VIRTUAL_SIZE;
#endif
	};
	virtual void	set_color				(const Fcolor& C)				{ color.set(C);				}
	virtual void	set_color				(float r, float g, float b)		{ color.set(r,g,b,1);		}
	virtual void	set_texture				(LPCSTR name);

	virtual	void	spatial_move			();
	virtual	Fvector	spatial_sector_point	();

	virtual IRender_Light*	dcast_Light		()	{ return this; }

	vis_data&		get_homdata				();
#if RENDER==R_R2
	void			gi_generate				();
	void			xform_calc				();
	void			vis_prepare				();
	void			vis_update				();
	void			_export 				(light_Package& dest);
#endif

	float			get_LOD					();

	light();
	virtual ~light();
};
