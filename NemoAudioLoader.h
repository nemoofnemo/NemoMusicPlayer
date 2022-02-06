#pragma once

#include "libnemo/nemo_utilities.h"
#include "player_utilities.h"
#include "windows.h"

class NemoAudioLoader
{
private:
	static const int TARGET_SAMPLE_RATE = 48000;
	static const AVSampleFormat TARGET_SAMPLE_FORMAT = AV_SAMPLE_FMT_FLT;

	AVFormatContext* format_context = nullptr;
	AVCodecContext* codec_context = nullptr;
	AVFrame* frame = nullptr;
	AVPacket* packet = nullptr;
	SwrContext* swr_ctx = nullptr;
	int stream_index = -1;

private:
	int decode_packet(nemo::ByteArray* arr);

public:
	NemoAudioLoader();
	NemoAudioLoader(const NemoAudioLoader&) = delete;
	NemoAudioLoader(NemoAudioLoader&&) = delete;
	void operator=(const NemoAudioLoader&) = delete;
	void operator=(NemoAudioLoader&&) = delete;
	virtual ~NemoAudioLoader();

	bool open(std::string path);
	bool load(nemo::ByteArray* arr, int* channel_count);
	int get_channel_count(void);
	void close(void);
};

