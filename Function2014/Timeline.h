#pragma once

#include "common.h"
#include "TimelineEventAbstract.h"

class Timeline : public IRenderable
{
public:
	Timeline()
	{
	}


	~Timeline()
	{
		for each (TimelineEventAbstract* timelineEvent in timelineEvents)
		{
			SAFE_DELETE(timelineEvent);
		}
	}

	void Timeline::Render(DWORD tick, ID3D11DeviceContext2 *m_pDeviceContext, IDXGISwapChain1 *m_pSwapChain)
	{
		for each (TimelineEventAbstract* timelineEvent in timelineEvents)
		{
			timelineEvent->Render(tick, m_pDeviceContext, m_pSwapChain);
		}
	}

public:
	std::vector<TimelineEventAbstract*> timelineEvents;
};

