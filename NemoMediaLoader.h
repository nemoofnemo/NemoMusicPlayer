#pragma once

#include "libnemo/nemo_utilities.h"
#include "FFmpegHeader.h"

class NemoMediaLoader
{
public:
	NemoMediaLoader();
	virtual ~NemoMediaLoader();

	bool open(std::string path, int buf_size);
};

