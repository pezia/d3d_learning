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

	void Timeline::Render(DWORD tick, const D3DResources* d3dResources)
	{
		for each (TimelineEventAbstract* timelineEvent in timelineEvents)
		{
			timelineEvent->Render(tick, d3dResources);
		}
	}

public:
	std::vector<TimelineEventAbstract*> timelineEvents;
};

