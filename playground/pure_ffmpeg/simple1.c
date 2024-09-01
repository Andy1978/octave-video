#include <stdio.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>


// required to look up the correct codec ID depending on the FOURCC code,
// this is just a snipped from the file riff.c from ffmpeg/libavformat
typedef struct AVCodecTag
{
  int id;
  unsigned int tag;
} AVCodecTag;

#define CV_PRINTABLE_CHAR(ch) ((ch) < 32 ? '?' : (ch))
#define CV_TAG_TO_PRINTABLE_CHAR4(tag) CV_PRINTABLE_CHAR((tag) & 255), CV_PRINTABLE_CHAR(((tag) >> 8) & 255), CV_PRINTABLE_CHAR(((tag) >> 16) & 255), CV_PRINTABLE_CHAR(((tag) >> 24) & 255)

/*
 * SNIPPET
 * ffmpeg -h muxer=webp
 *
 */

/*
// enumerate all codecs and put into list
std::vector<AVCodec*> encoderList;
AVCodec * codec = nullptr;
while (codec = av_codec_next(codec))
{
    // try to get an encoder from the system
    auto encoder = avcodec_find_encoder(codec->id);
    if (encoder)
    {
        encoderList.push_back(encoder);
    }
}
// enumerate all containers
AVOutputFormat * outputFormat = nullptr;
while (outputFormat = av_oformat_next(outputFormat))
{
    for (auto codec : encoderList)
    {
        // only add the codec if it can be used with this container
        if (avformat_query_codec(outputFormat, codec->id, FF_COMPLIANCE_STRICT) == 1)
        {
            // add codec for container
        }
    }
}
*/

void dump_tags (const AVCodecTag *const *tags)
{
  printf ("tags        = %p\n", tags);

  int i;
  for (i = 0; tags && tags[i]; i++)
    {
      const AVCodecTag * ptags = tags[i];
      printf ("ptags = %p\n", ptags);
      while (ptags->id != AV_CODEC_ID_NONE)
        {
          printf("tag = 0x%08x = '%c%c%c%c' codec_id %5i = '%s'\n", ptags->tag, CV_TAG_TO_PRINTABLE_CHAR4(ptags->tag), ptags->id, avcodec_get_name(ptags->id));
          //printf("'%c%c%c%c'\n", CV_TAG_TO_PRINTABLE_CHAR4(ptags->tag));
          ptags++;
        }
    }


}

/*
  Es gibt die AVCodecID

  av_codec_get_id()
    looped über const struct AVCodecTag *const *tags, gibt den ersten Treffer zurück
  av_codec_get_tag
    looped über const struct AVCodecTag *const *tags, gibt den ersten Treffer zurück

*/

int main ()
{
  const AVOutputFormat *fmt = NULL;
  const struct AVCodecTag *table[] = { avformat_get_riff_video_tags(), avformat_get_mov_video_tags(), 0 };

#if 1
  void *i = 0;
  while ((fmt = av_muxer_iterate(&i)))
    {
      //fmt = av_guess_format(NULL, "foo.mp4", NULL);

      printf ("name        = '%s'\n", fmt->name);
      printf ("long_name   = '%s'\n", fmt->long_name);
      printf ("mime_type   = '%s'\n", fmt->mime_type);
      printf ("extensions  = '%s'\n", fmt->extensions);
      //printf ("audio_codec = %i = '%s'\n", fmt->audio_codec, avcodec_get_name (fmt->audio_codec));
      printf ("video_codec = %i = '%s'\n", fmt->video_codec, avcodec_get_name (fmt->video_codec));
      printf ("codec_tag   = %p\n", fmt->codec_tag);

      // über codec_tag loopen
      //dump_tags (fmt->codec_tag);

      unsigned int ret = av_codec_get_tag (fmt->codec_tag, fmt->video_codec);
      //printf ("tag = %u\n", ret);
      printf("tag = 0x%08x = '%c%c%c%c'\n", ret, CV_TAG_TO_PRINTABLE_CHAR4(ret));

      // alternative?
      ret = av_codec_get_tag (table, fmt->video_codec);
      //printf ("tag = %u\n", ret);
      printf("tag = 0x%08x = '%c%c%c%c'\n", ret, CV_TAG_TO_PRINTABLE_CHAR4(ret));

      printf ("\n");
    }
#endif

// über RIFF loopen
#if 0
  const struct AVCodecTag *table[] = { avformat_get_riff_video_tags(), avformat_get_mov_video_tags(), 0 };
  dump_tags (table);
#endif

  //fmt = av_guess_format (NULL, "foo.webp", NULL); //wmf, wma
  //dump_tags (fmt->codec_tag);

  //enum AVCodecID id = av_codec_get_id (fmt->codec_tag, MKTAG('H', '5', '6', '3'));
  //printf ("id = %i, name = '%s'\n", id, avcodec_get_name (id));

  /*
   uint32_t tag = MKTAG('H', '2', '6', '4');
   const struct AVCodecTag *table[] = { avformat_get_riff_video_tags(), 0 };
   enum AVCodecID id = av_codec_get_id(table, tag);
  */

  /*
  	std::vector<AVCodec*> encoderList;
  	AVCodec * codec = nullptr;
  	while (codec = av_codec_next(codec))
  	{
  		// try to get an encoder from the system
  		auto encoder = avcodec_find_encoder(codec->id);
  		if (encoder)
  		{
  			//encoderList.push_back(encoder);
  		}
  	}
  */

  // Ein paar Funktionen ausprobieren
  //unsigned int ret = av_codec_get_tag (fmt->codec_tag, AV_CODEC_ID_WEBP);
  //printf ("ret = %u\n", ret);

  //int ret2 = av_codec_get_tag2(fmt->codec_tag, AV_CODEC_ID_WEBP, &ret);
  //printf ("ret2 = %i, ret = %u\n", ret2, ret);

  //const AVCodecTag *const *tags = fmt->codec_tag;
  //printf ("tags        = %p\n", tags);

  //enum AVCodecID id = av_codec_get_id(tags, 0x6134706d);

  // macros.h: #define MKTAG(a,b,c,d)   ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
  // MKTAG('H', 'a', 'p', '1');
  // siehe auch MKBETAG


  return 0;
}

