#pragma once
class CDetectable
{
protected:
	shared_str	m_detectorID;
public:
	shared_str	DetectorID() { return m_detectorID; };
	void		DetectorID_Load(shared_str sect, shared_str id_to_read = "detector_id", shared_str fallback = "no_detect");
	virtual bool		IsVisible() {
		return true;
	};
};

