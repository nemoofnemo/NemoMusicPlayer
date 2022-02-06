#include "player_utilities.h"

int64_t ms_to_ts(unsigned long long ms, AVRational time_base)
{
	return int64_t(double(ms) * time_base.den / 1000.0 / time_base.num);
}

unsigned long long ts_to_ms(int64_t ts, AVRational time_base)
{
	return unsigned long long(double(ts) * 1000.0 * time_base.num / time_base.den);
}
