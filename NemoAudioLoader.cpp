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

    int cur_buf_len = 0;
    int max_buf_len = 0;
    uint8_t** buf = nullptr;
    int linesize = 0;
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
        cur_buf_len = av_rescale_rnd(frame->nb_samples, 
            TARGET_SAMPLE_RATE, frame->sample_rate, AV_ROUND_UP);
        if (cur_buf_len > max_buf_len) {
            if (buf) {
                av_freep(&buf[0]);
                ret = av_samples_alloc(buf, &linesize, channel_count,
                    cur_buf_len, TARGET_SAMPLE_FORMAT, 1);
                if (ret < 0) {
                    nDebug("av_samples_alloc error");
                    av_frame_unref(frame);
                    return ret;
                }
                max_buf_len = cur_buf_len;
            }
            else {
                av_samples_alloc_array_and_samples(&buf, &linesize,
                    channel_count, 2048, TARGET_SAMPLE_FORMAT, 0);
                max_buf_len = 2048;
            }          
        }

        ret = swr_convert(swr_ctx, buf, cur_buf_len,
            (const uint8_t**)frame->data, frame->nb_samples);
        if (ret < 0) {
            nDebug("swr_convert error");
            av_frame_unref(frame);
            if (buf) {
                av_freep(&buf[0]);
                av_freep(&buf);
            }
            return ret;
        }

        auto len = av_samples_get_buffer_size(&linesize, channel_count,
            ret, TARGET_SAMPLE_FORMAT, 1);
        if (ret < 0) {
            nDebug("av_samples_get_buffer_size error");
            av_frame_unref(frame);
            if (buf) {
                av_freep(&buf[0]);
                av_freep(&buf);
            }
            return ret;
        }

        if(buf)
            arr->append(buf[0], len);

        av_frame_unref(frame);
    }

    if (buf) {
        av_freep(&buf[0]);
        av_freep(&buf);
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
    if (format_context) {
        this->close();
    }

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
    codec_context = avcodec_alloc_context3(dec);
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

    channel_count = av_get_channel_layout_nb_channels(
        codec_context->channel_layout);

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

bool NemoAudioLoader::seek(std::chrono::milliseconds ms)
{
    if (!format_context)
        return false;
    if (stream_index < 0)
        return false;

    auto target = ms_to_ts(ms.count(), 
        format_context->streams[stream_index]->time_base);
    auto ret = av_seek_frame(format_context, stream_index, 
        target, AVSEEK_FLAG_ANY);

    return ret >= 0;
}

int64_t NemoAudioLoader::load(nemo::ByteArray* arr, int64_t ts)
{
    return int64_t();
}

bool NemoAudioLoader::load(nemo::ByteArray* arr)
{
    if (!arr)
        return false;

    if (stream_index < 0)
        return false;

    int ret = 0;
    /* read frames from the file */
    while (av_read_frame(format_context, packet) >= 0) {
        // check if the packet belongs to a stream we are interested in, otherwise skip it
        if (packet->stream_index == stream_index)
            ret = decode_packet(arr);
        av_packet_unref(packet);
        if (ret < 0)
            break;
    }

    /* flush the decoders */
    decode_packet(arr);
    
    return true;
}

int NemoAudioLoader::get_channel_count(void)
{
    if (!codec_context)
        return 0;
    return av_get_channel_layout_nb_channels(
        codec_context->channel_layout);
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
    channel_count = 0;
}
