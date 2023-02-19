#include "stdafx.h"
#include "CDetectable.h"

void CDetectable::DetectorID_Load(shared_str sect, shared_str id_to_read, shared_str fallback)
{
	if (pSettings->line_exist(sect, id_to_read))
	{
		m_detectorID = pSettings->r_string(sect, id_to_read.c_str());
	}
	else
		m_detectorID = fallback;
}
