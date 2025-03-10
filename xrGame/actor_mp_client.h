#ifndef ACTOR_MP_CLIENT_H
#define ACTOR_MP_CLIENT_H

#include "actor.h"
#include "actor_mp_state.h"

class CActorMP : public CActor {
private:
	typedef CActor			inherited;

private:
	actor_mp_state_holder	m_state_holder;
	bool					m_i_am_dead;

private:
			void			fill_state			(actor_mp_state &state);
			void			process_packet		(net_update		&N);
			void			postprocess_packet	(net_update_A	&packet);

public:
							CActorMP			();
	virtual void			net_Export			(NET_Packet		&packet);
	virtual void			net_Import			(NET_Packet		&packet);
	virtual BOOL			net_Relevant		();
	virtual void			OnEvent				(NET_Packet		&packet, u16 type);
	virtual void			Die					(CObject		*killer);

	void			use_booster(NET_Packet& packet);
};

#endif // ACTOR_MP_CLIENT_H