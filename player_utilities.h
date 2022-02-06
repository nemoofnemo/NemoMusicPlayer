#pragma once
#include "FFmpegHeader.h"

int64_t ms_to_ts(unsigned long long ms, AVRational time_base);
unsigned long long ts_to_ms(int64_t ts, AVRational time_base);
