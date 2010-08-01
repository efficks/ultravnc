#include "stdhdrs.h"

class critsec
{
public:
	critsec(CRITICAL_SECTION * IN_cs)
		{
			cs=IN_cs;
			EnterCriticalSection( cs );
		};
	~critsec()
	{
		LeaveCriticalSection( cs );
	};
private:
	CRITICAL_SECTION * cs;
};