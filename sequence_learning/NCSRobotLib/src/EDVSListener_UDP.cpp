#include <EDVSListener_UDP.h>

void EDVSListener_UDP::receivedNewEDVSEvent(EDVSEvent &e)
{
    if (--delay < 0) {
			delay = TIMESTAMP_RENEWAL_DELAY;
			// time since listening started, in ms
			duration = pt::microsec_clock::local_time() - start;
			timestamp = duration.total_milliseconds();
		}
		sprintf(buffer, "%ld,%d,%d,%d\n", timestamp, e.m_x, e.m_y, e.m_pol);
		send(buffer);
}
