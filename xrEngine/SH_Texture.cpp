#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"

#ifndef _EDITOR
#include "render.h"
#endif

#include "tntQAVI.h"
#include "xrTheora_Surface.h"

#define		PRIORITY_HIGH	12
#define		PRIORITY_NORMAL	8
#define		PRIORITY_LOW	4

typedef int (WINAPI* OBJECT_METHOD) (PVOID pThis);

OBJECT_METHOD ENGINE_API TextureLoadCapture = NULL;
OBJECT_METHOD ENGINE_API TextureUnloadCapture = NULL;

void resptrcode_texture::create(LPCSTR _name)
{
	_set(Device.Resources->_CreateTexture(_name));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTexture::CTexture()
{
	pSurface = NULL;
	pAVI = NULL;
	pTheora = NULL;
	desc_cache = 0;
	seqMSPF = 0;
	flags.MemoryUsage = 0;
	flags.bLoaded = false;
	flags.bUser = false;
	flags.seqCycles = FALSE;
	m_material = 1.0f;
	bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_load);
}

CTexture::~CTexture()
{
	Unload();

	// release external reference
	Device.Resources->_DeleteTexture(this);
}

void					CTexture::surface_set(IDirect3DBaseTexture9* surf)
{
	if (surf)			surf->AddRef();
	_RELEASE(pSurface);

	pSurface = surf;
}

IDirect3DBaseTexture9* CTexture::surface_get()
{
	if (pSurface)		pSurface->AddRef();
	return pSurface;
}

void CTexture::PostLoad()
{
	if (pTheora)				bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_theora);
	else if (pAVI)				bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_avi);
	else if (!seqDATA.empty())	bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_seq);
	else						bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_normal);
}

void CTexture::apply_load(u32 dwStage) {
	if (!flags.bLoaded)		Load();
	else					PostLoad();
	bind(dwStage);
};

void CTexture::apply_theora(u32 dwStage) {
	if (pTheora->Update(m_play_time != 0xFFFFFFFF ? m_play_time : Device.dwTimeContinual)) {
		R_ASSERT(D3DRTYPE_TEXTURE == pSurface->GetType());
		IDirect3DTexture9* T2D = (IDirect3DTexture9*)pSurface;
		D3DLOCKED_RECT		R;
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = pTheora->Width(true);
		rect.bottom = pTheora->Height(true);

		u32 _w = pTheora->Width(false);

		R_CHK(T2D->LockRect(0, &R, &rect, 0));
		R_ASSERT(R.Pitch == int(pTheora->Width(false) * 4));
		int _pos = 0;
		pTheora->DecompressFrame((u32*)R.pBits, _w - rect.right, _pos);
		VERIFY(u32(_pos) == rect.bottom * _w);
		R_CHK(T2D->UnlockRect(0));
	}
	CHK_DX(HW.pDevice->SetTexture(dwStage, pSurface));
};
void CTexture::apply_avi(u32 dwStage) {
	if (pAVI->NeedUpdate()) {
		R_ASSERT(D3DRTYPE_TEXTURE == pSurface->GetType());
		IDirect3DTexture9* T2D = (IDirect3DTexture9*)pSurface;

		// AVI
		D3DLOCKED_RECT R;
		R_CHK(T2D->LockRect(0, &R, NULL, 0));
		R_ASSERT(R.Pitch == int(pAVI->m_dwWidth * 4));
		//		R_ASSERT(pAVI->DecompressFrame((u32*)(R.pBits)));
		BYTE* ptr; pAVI->GetFrame(&ptr);
		CopyMemory(R.pBits, ptr, pAVI->m_dwWidth * pAVI->m_dwHeight * 4);
		//		R_ASSERT(pAVI->GetFrame((BYTE*)(&R.pBits)));

		R_CHK(T2D->UnlockRect(0));
	}
	CHK_DX(HW.pDevice->SetTexture(dwStage, pSurface));
};
void CTexture::apply_seq(u32 dwStage) {
	// SEQ
	u32	frame = Device.dwTimeContinual / seqMSPF; //Device.dwTimeGlobal
	u32	frame_data = seqDATA.size();
	if (flags.seqCycles) {
		u32	frame_id = frame % (frame_data * 2);
		if (frame_id >= frame_data)	frame_id = (frame_data - 1) - (frame_id % frame_data);
		pSurface = seqDATA[frame_id];
	}
	else {
		u32	frame_id = frame % frame_data;
		pSurface = seqDATA[frame_id];
	}
	CHK_DX(HW.pDevice->SetTexture(dwStage, pSurface));
};
void CTexture::apply_normal(u32 dwStage) {
	CHK_DX(HW.pDevice->SetTexture(dwStage, pSurface));
};

void CTexture::Preload()
{
	// Material
/*
	if (Device.Resources->m_description->line_exist("specification",*cName))	{
//		if (strstr(*cName,"ston_stena"))	__asm int 3;
		LPCSTR		descr			=	Device.Resources->m_description->r_string("specification",*cName);
		string256	bmode;
		sscanf		(descr,"bump_mode[%[^]]], material[%f]",bmode,&m_material);
		if ((bmode[0]=='u')&&(bmode[1]=='s')&&(bmode[2]=='e')&&(bmode[3]==':'))
		{
			// bump-map specified
			m_bumpmap		=	bmode+4;
		}
//		Msg	("mid[%f] : %s",m_material,*cName);
	}
*/
	m_bumpmap = Device.Resources->m_textures_description.GetBumpName(cName);
	m_material = Device.Resources->m_textures_description.GetMaterial(cName);
}

void CTexture::Load()
{
	int result = 0;
	if (TextureLoadCapture)
		result = TextureLoadCapture((PVOID)this);

	if (result <= 0)
		LoadImpl();
}

void CTexture::LoadImpl()
{
	flags.bLoaded = true;
	desc_cache = 0;
	if (pSurface)					return;

	flags.bUser = false;
	flags.MemoryUsage = 0;
	if (0 == stricmp(*cName, "$null"))	return;
	if (0 != strstr(*cName, "$user$")) {
		flags.bUser = true;
		return;
	}

	Preload();
#ifndef		DEDICATED_SERVER
	// Check for OGM
	string_path			fn;
	string_path			name;
	strcpy_s(name, sizeof(name), *cName);

	char* sub = strstr(name, "|");
	if (sub)
		sub[0] = 0;

	if (FS.exist(fn, "$game_textures$", name, ".ogm")) {
		// AVI
		pTheora = xr_new<CTheoraSurface>();
		m_play_time = 0xFFFFFFFF;

		if (!pTheora->Load(fn)) {
			xr_delete(pTheora);
			FATAL("Can't open video stream");
		}
		else {
			flags.MemoryUsage = pTheora->Width(true) * pTheora->Height(true) * 4;
			pTheora->Play(TRUE, Device.dwTimeContinual);

			// Now create texture
			IDirect3DTexture9* pTexture = 0;
			u32 _w = pTheora->Width(false);
			u32 _h = pTheora->Height(false);

			HRESULT hrr = HW.pDevice->CreateTexture(
				_w, _h, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL);

			pSurface = pTexture;
			if (FAILED(hrr))
			{
				FATAL("Invalid video stream");
				R_CHK(hrr);
				xr_delete(pTheora);
				pSurface = 0;
			}
		}
	}
	else
		if (FS.exist(fn, "$game_textures$", name, ".avi")) {
			// AVI
			pAVI = xr_new<CAviPlayerCustom>();

			if (!pAVI->Load(fn)) {
				xr_delete(pAVI);
				FATAL("Can't open video stream");
			}
			else {
				flags.MemoryUsage = pAVI->m_dwWidth * pAVI->m_dwHeight * 4;

				// Now create texture
				IDirect3DTexture9* pTexture = 0;
				HRESULT hrr = HW.pDevice->CreateTexture(
					pAVI->m_dwWidth, pAVI->m_dwHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
					&pTexture, NULL
				);
				pSurface = pTexture;
				if (FAILED(hrr))
				{
					FATAL("Invalid video stream");
					R_CHK(hrr);
					xr_delete(pAVI);
					pSurface = 0;
				}
			}
		}
		else
			if (FS.exist(fn, "$game_textures$", name, ".seq"))
			{
				// Sequence
				string256 buffer;
				IReader* _fs = FS.r_open(fn);

				flags.seqCycles = FALSE;
				_fs->r_string(buffer, sizeof(buffer));
				if (0 == stricmp(buffer, "cycled"))
				{
					flags.seqCycles = TRUE;
					_fs->r_string(buffer, sizeof(buffer));
				}
				u32 fps = atoi(buffer);
				seqMSPF = 1000 / fps;

				while (!_fs->eof())
				{
					_fs->r_string(buffer, sizeof(buffer));
					_Trim(buffer);
					if (buffer[0])
					{
						// Load another texture
						u32	mem = 0;
						pSurface = ::Render->texture_load(buffer, mem);
						if (pSurface)
						{
							// pSurface->SetPriority	(PRIORITY_LOW);
							seqDATA.push_back(pSurface);
							flags.MemoryUsage += mem;
						}
					}
				}
				pSurface = 0;
				FS.r_close(_fs);
			}
			else
			{
				// Normal texture
				u32	mem = 0;
				pSurface = ::Render->texture_load(name, mem);

				// Calc memory usage and preload into vid-mem
				if (pSurface) {
					// pSurface->SetPriority	(PRIORITY_NORMAL);
					flags.MemoryUsage = mem;
				}
			}
#endif
	PostLoad();
}

void CTexture::Unload()
{
#ifdef DEBUG
	string_path				msg_buff;
	sprintf_s(msg_buff, sizeof(msg_buff), "* Unloading texture [%s] pSurface RefCount=", cName.c_str());
#endif // DEBUG

	//.	if (flags.bLoaded)		Msg		("* Unloaded: %s",cName.c_str());

	flags.bLoaded = FALSE;
	if (!seqDATA.empty()) {
		for (u32 I = 0; I < seqDATA.size(); I++)
		{
			_RELEASE(seqDATA[I]);
		}
		seqDATA.clear();
		pSurface = 0;
	}

#ifdef DEBUG
	_SHOW_REF(msg_buff, pSurface);
#endif // DEBUG
	if (TextureUnloadCapture)
		TextureUnloadCapture((PVOID)this);
	_RELEASE(pSurface);

	xr_delete(pAVI);
	xr_delete(pTheora);

	bind = fastdelegate::FastDelegate1<u32>(this, &CTexture::apply_load);
}

void CTexture::desc_update()
{
	desc_cache = pSurface;
	if (pSurface && (D3DRTYPE_TEXTURE == pSurface->GetType()))
	{
		IDirect3DTexture9* T = (IDirect3DTexture9*)pSurface;
		R_CHK(T->GetLevelDesc(0, &desc));
	}
}

void CTexture::video_Play(BOOL looped, u32 _time)
{
	if (pTheora) pTheora->Play(looped, (_time != 0xFFFFFFFF) ? (m_play_time = _time) : Device.dwTimeContinual);
}

void CTexture::video_Pause(BOOL state)
{
	if (pTheora) pTheora->Pause(state);
}

void CTexture::video_Stop()
{
	if (pTheora) pTheora->Stop();
}

BOOL CTexture::video_IsPlaying()
{
	return (pTheora) ? pTheora->IsPlaying() : FALSE;
}