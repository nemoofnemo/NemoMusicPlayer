#include "NemoAudioLoader.h"

int NemoAudioLoader::decode_packet(nemo::ByteArray* arr)
{
    int ret = 0;

    // submit the packet to the decoder
    ret = avcodec_send_packet(codec_context, packet);
    if (ret < 0) {
        nDebug("Error submitting a packet for decoding");
        //nDebug(av_err2str(ret)));
        return ret;
    }

    // get all the available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(codec_context, frame);
        if (ret < 0) {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;

            //fprintf(stderr, "Error during decoding (%s)\n", av_err2str(ret));
            nDebug("Error during decoding");
            return ret;
        }

        // write the frame data
        

        av_frame_unref(frame);
        if (ret < 0)
            return ret;
    }

    return 0;
}

NemoAudioLoader::NemoAudioLoader()
{
}

NemoAudioLoader::~NemoAudioLoader()
{
    this->close();
}

bool NemoAudioLoader::open(std::string path)
{
    /* open input file, and allocate format context */
    if (avformat_open_input(&format_context, path.c_str(), NULL, NULL) < 0) {
        nDebug("Could not open source file");
        return false;
    }

    /* retrieve stream information */
    if (avformat_find_stream_info(format_context, NULL) < 0) {
        nDebug("Could not find stream information");
        return false;
    }

    int ret = av_find_best_stream(format_context,
        AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (ret < 0) {
        nDebug("Could not find stream in input file");
        return false;
    }
    stream_index = ret;

    /* find decoder for the stream */
    auto st = format_context->streams[stream_index];
    auto dec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!dec) {
        nDebug("Failed to find codec");
        return false;
    }

    /* Allocate a codec context for the decoder */
    auto codec_context = avcodec_alloc_context3(dec);
    if (!codec_context) {
        nDebug("Failed to allocate the %s codec context");
        return false;
    }

    /* Copy codec parameters from input stream to output codec context */
    if ((ret = avcodec_parameters_to_context(codec_context, st->codecpar)) < 0) {
        nDebug("Failed to copy codec parameters to decoder context");
        return false;
    }

    /* Init the decoders */
    if ((ret = avcodec_open2(codec_context, dec, NULL)) < 0) {
        nDebug("Failed to open codec");
        return false;
    }

    frame = av_frame_alloc();
    if (!frame) {
        nDebug("Could not allocate frame");
        return false;
    }

    packet = av_packet_alloc();
    if (!packet) {
        nDebug("Could not allocate packet");
        return false;
    }

    /* create resampler context */
    swr_ctx = swr_alloc();
    if (!swr_ctx) {
        nDebug("Could not allocate resampler context");
        return false;
    }

    /* set options */
    av_opt_set_int(swr_ctx, "in_channel_layout", codec_context->channel_layout, 0);
    av_opt_set_int(swr_ctx, "in_sample_rate", codec_context->sample_rate, 0);
    av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", codec_context->sample_fmt, 0);

    av_opt_set_int(swr_ctx, "out_channel_layout", codec_context->channel_layout, 0);
    av_opt_set_int(swr_ctx, "out_sample_rate", TARGET_SAMPLE_RATE, 0);
    av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", TARGET_SAMPLE_FORMAT, 0);

    /* initialize the resampling context */
    if ((ret = swr_init(swr_ctx)) < 0) {
        nDebug("Failed to initialize the resampling context");
        return false;
    }

    return true;
}

bool NemoAudioLoader::load(nemo::ByteArray* arr, int* channel_count)
{
    if (!arr || !channel_count)
        return false;

    if (stream_index < 0)
        return false;

    ///* read frames from the file */
    //while (av_read_frame(format_context, packet) >= 0) {
    //    // check if the packet belongs to a stream we are interested in, otherwise skip it
    //    if (packet->stream_index == stream_index)
    //        ret = decode_packet(arr);
    //    av_packet_unref(packet);
    //    if (ret < 0)
    //        break;
    //}

    ///* flush the decoders */
    //decode_packet(arr);
    
}

int NemoAudioLoader::get_channel_count(void)
{
    if (!codec_context)
        return 0;
    return av_get_channel_layout_nb_channels(
        codec_context->channel_layout);;
}

void NemoAudioLoader::close(void)
{
    if (swr_ctx)
        swr_free(&swr_ctx);
    if (frame)
        av_frame_free(&frame);
    if (packet)
        av_packet_free(&packet);
    if(codec_context)
        avcodec_free_context(&codec_context);
    if(format_context)
        avformat_close_input(&format_context);
    stream_index = -1;
}
