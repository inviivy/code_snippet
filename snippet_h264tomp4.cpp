extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/mem.h>
}

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "remux.hpp"

struct bufSpan {
  const char *ptr;
  size_t len;
};

void func() {
  std::string path = u8"/home/eric/test_video/h264.h264";
  std::ifstream in(path, std::ios::binary | std::ios::in);
  assert(in.is_open());

  constexpr size_t MEM_BUFFER_LEN = 16 * 1024 * 1024;
  char *file_content = new char[MEM_BUFFER_LEN];
  in.read(file_content, MEM_BUFFER_LEN);
  bufSpan span{.ptr = file_content, .len = static_cast<size_t>(in.gcount())};

  // 用于avio的内存
  constexpr size_t VIDEO_BUFFER_LEN = 8 * 1024 * 1024;
  uint8_t *file_buffer = static_cast<uint8_t *>(::av_malloc(VIDEO_BUFFER_LEN));

  ::AVFormatContext *ifmt_ctx = ::avformat_alloc_context(), *ofmt_ctx = nullptr;
  assert(ifmt_ctx != nullptr);

  // 申请avio的context
  ::AVIOContext *avio = ::avio_alloc_context(
      file_buffer, VIDEO_BUFFER_LEN, 0, &span,
      [](void *opaque, uint8_t *buf, int buf_size) -> int {
        auto span = static_cast<bufSpan *>(opaque);

        std::cout << "buf_size = " << buf_size << '\n';
        std::cout << "span->len = " << span->len << '\n';

        auto minLen = std::min(span->len, static_cast<size_t>(buf_size));
        std::cout << "read " << buf_size << " bytes\n";
        if (minLen > 0) {
          ::memcpy(buf, span->ptr, minLen);
          span->ptr += minLen;
          span->len -= minLen;
          return minLen;
        } else {
          return AVERROR_EOF;
        }
      },
      nullptr, nullptr);
  assert(avio != nullptr);

  // 绑定avio 到 ifmt_ctx
  ifmt_ctx->pb = avio;

  // 定义 input format和output format
  // 已知ifmt是h264
  const AVInputFormat *ifmt = nullptr;
  int ret2 = av_probe_input_buffer(ifmt_ctx->pb, &ifmt, nullptr, nullptr, 0, 0);
  if (ret2 < 0) {
    char buf[512];
    av_strerror(ret2, buf, sizeof(buf));
    std::cout << buf << '\n';
    std::abort();
  }
  assert(strcmp(ifmt->name, "h264") == 0);

  AVOutputFormat *ofmt = nullptr;

  // 打开ifmt_ctx
  int ret = avformat_open_input(&ifmt_ctx, nullptr, ifmt, nullptr);
  assert(ret == 0);

  // 尝试find_stream
  ret = avformat_find_stream_info(ifmt_ctx, nullptr);
  assert(ret >= 0);

  // 输出stream 0的信息, 因为只有一条264
  av_dump_format(ifmt_ctx, 0, nullptr, 0);

  // 分配outformat context
  const char *outFileName = "out.mp4";
  avformat_alloc_output_context2(&ofmt_ctx, nullptr, "mp4", outFileName);
  assert(ofmt_ctx != nullptr);

  // av_free(avio->buffer);
  // avio_context_free(&avio);
  // delete[] file_content;
  // avformat_free_context(ifmt_ctx);
  // avformat_free_context(ofmt_ctx);

  // 开始遍历input format
  for (int i = 0; i < ifmt_ctx->nb_streams; ++i) {
    AVStream *stream = ifmt_ctx->streams[i];
    AVStream *outStream = nullptr;
    if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      outStream = avformat_new_stream(ofmt_ctx, nullptr);
      assert(outStream != nullptr);
      // 把解码器复制过去
      ret = avcodec_parameters_copy(outStream->codecpar, stream->codecpar);
      assert(ret >= 0);
      outStream->codecpar->codec_tag = 0;

      std::cout << "stream->codecpar->width = " << stream->codecpar->width
                << '\n';
      std::cout << "stream->codecpar->height = " << stream->codecpar->height
                << '\n';
    }
  }

  std::cout << "ofmt_ctx->oformat->flags:" << ofmt_ctx->oformat->flags << '\n';
  if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
    std::cout << "=============>!AVFMT_NOFILE\n";
    ret = avio_open(&ofmt_ctx->pb, outFileName, AVIO_FLAG_WRITE);
    assert(ret >= 0);
  }

  av_free(avio->buffer);
  avio_context_free(&avio);
  delete[] file_content;
  avformat_free_context(ifmt_ctx);
  if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
    avio_close(ofmt_ctx->pb);
  }

  avformat_free_context(ofmt_ctx);

  /*
  // 理论上只有一条h264
  // 试着dump outformat context的信息
  std::cout << "<=============================>\n";
  av_dump_format(ofmt_ctx, 0, outFileName, 1);

  // 写入文件头
  // ret = avformat_write_header(ofmt_ctx, nullptr);
  // assert(ret >= 0);

  std::cout << "===================================>\n";

  av_free(avio->buffer);
  avio_context_free(&avio);
  delete[] file_content;
  avformat_free_context(ifmt_ctx);
  avformat_free_context(ofmt_ctx);


  // 定义要写入的帧
  int totalFrame = 25 * 10;
  int frame_index = 0;
  AVPacket pkt;
  // while (frame_index < totalFrame) {

  //   // 这里假设instream就是 ifmt_ctx->stream[0]
  //   AVStream *in_stream = ifmt_ctx->streams[0];
  //   AVStream *out_stream = ofmt_ctx->streams[0];
  //   // assert(pkt.stream_index == 0);
  //   ret = av_read_frame(ifmt_ctx, &pkt);
  //   if (ret < 0) {
  //     // 文件尾
  //     break;
  //   }
  //   if (pkt.stream_index == AVMEDIA_TYPE_VIDEO) {
  //     // FIX£ºNo PTS (Example: Raw H.264)
  //     // Simple Write PTS
  //     if (pkt.pts == AV_NOPTS_VALUE) {
  //       // Write PTS
  //       AVRational time_base1 = in_stream->time_base;
  //       // Duration between 2 frames (¦Ìs)
  //       int64_t calc_duration =
  //           (double)AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
  //       // Parameters
  //       pkt.pts = (double)(frame_index * calc_duration) /
  //                 (double)(av_q2d(time_base1) * AV_TIME_BASE);
  //       pkt.dts = pkt.pts;
  //       pkt.duration =
  //           (double)calc_duration / (double)(av_q2d(time_base1) *
  AV_TIME_BASE);
  //       frame_index++;
  //     }

  //     // copy packet
  //     AVRounding rnd =
  //         (AVRounding)((int)AV_ROUND_NEAR_INF | (int)AV_ROUND_PASS_MINMAX);
  //     pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base,
  //                                out_stream->time_base, rnd);
  //     pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base,
  //                                out_stream->time_base, rnd);
  //     pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base,
  //                                 out_stream->time_base);
  //     pkt.pos = -1;

  //     ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
  //     if (ret < 0) {
  //       av_packet_unref(&pkt);
  //       break;
  //     }
  //     av_packet_unref(&pkt);
  //     ++frame_index;
  //   }
  // }

  // 写入文件
  // assert(av_write_trailer(ofmt_ctx) == 0);

  if (ofmt_ctx->pb) {
    avio_context_free(&ofmt_ctx->pb);
  }
  avformat_free_context(ofmt_ctx);

  avformat_close_input(&ifmt_ctx);



av_free(avio->buffer);
  avio_context_free(&avio);

  // 释放内存
  avformat_free_context(ifmt_ctx);

  delete[] file_content;

  in.close();
  */
}

int main() {
  func();
  return 0;
}
