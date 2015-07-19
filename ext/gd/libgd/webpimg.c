/*===========================================================================*
 - Copyright 2010 Google Inc.
 -
 - This code is licensed under the same terms as WebM:
 - Software License Agreement:  http://www.webmproject.org/license/software/
 - Additional IP Rights Grant:  http://www.webmproject.org/license/additional/
 *===========================================================================*/

/*
 * Encoding/Decoding of WebP still image compression format.
 *
 * 1. WebPDecode: Takes an array of bytes (string) corresponding to the WebP
 *                encoded image and generates output in the YUV format with
 *                the color components U, V subsampled to 1/2 resolution along
 *                each dimension.
 *
 * 2. YUV420toRGBA: Converts from YUV (with color subsampling) such as produced
 *                  by the WebPDecode routine into 32 bits per pixel RGBA data
 *                  array. This data array can be directly used by the Leptonica
 *                  Pix in-memory image format.
 *
 * 3. WebPEncode: Takes a Y, U, V data buffers (with color components U and V
 *                subsampled to 1/2 resolution) and generates the WebP string
 *
 * 4. RGBAToYUV420: Generates Y, U, V data (with color subsampling) from 32 bits
 *                  per pixel RGBA data buffer. The resulting YUV data can be
 *                  directly fed into the WebPEncode routine.
 *
 * 5. AdjustColorspace:
 *
 * 6. AdjustColorspaceBack:
 */
#include "gd.h"
#ifdef HAVE_LIBVPX
#include "webpimg.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "vpx/vpx_decoder.h"
#include "vpx/vp8dx.h"
#include "vpx/vpx_encoder.h"
#include "vpx/vp8cx.h"
#include "gd.h"

/*---------------------------------------------------------------------*
 *                              color conversions                      *
 *---------------------------------------------------------------------*/

#ifndef inline
# define inline __inline
#endif
static inline int clip(float v, int a, int b) {
  return (v > b) ? b : (v < 0) ? 0 : (int)(v);
}
enum {
    COLOR_RED = 1,
    COLOR_GREEN = 2,
    COLOR_BLUE = 3,
    ALPHA_CHANNEL = 0
};

/* endian neutral extractions of ARGB from a 32 bit pixel */
static const uint32  RED_SHIFT =
       8 * (sizeof(uint32) - 1 - COLOR_RED);           /* 16 */
static const uint32  GREEN_SHIFT =
       8 * (sizeof(uint32) - 1 - COLOR_GREEN);         /*  8 */
static const uint32  BLUE_SHIFT =
       8 * (sizeof(uint32) - 1 - COLOR_BLUE);          /*  0 */
static const uint32  ALPHA_SHIFT =
       8 * (sizeof(uint32) - 1 - ALPHA_CHANNEL);       /* 24 */

static inline int GetRed(const uint32* rgba) {
	return gdTrueColorGetRed(*rgba);
}

static inline int GetGreen(const uint32* rgba) {
	return gdTrueColorGetGreen(*rgba);
}

static inline int GetBlue(const uint32* rgba) {
  	return gdTrueColorGetBlue(*rgba);
}

enum { YUV_FRAC = 16 };

static inline int clip_uv(int v) {
   v = (v + (257 << (YUV_FRAC + 2 - 1))) >> (YUV_FRAC + 2);
   return ((v & ~0xff) == 0) ? v : v < 0 ? 0u : 255u;
}


/* YUV <-----> RGB conversions */
/* The exact naming is Y'CbCr, following the ITU-R BT.601 standard.
 * More information at: http://en.wikipedia.org/wiki/YCbCr
 */
static inline int GetLumaY(int r, int g, int b) {
  const int kRound = (1 << (YUV_FRAC - 1)) + (16 << YUV_FRAC);
  // Y = 0.2569 * R + 0.5044 * G + 0.0979 * B + 16
  const int luma = 16839 * r + 33059 * g + 6420 * b;
  return (luma + kRound) >> YUV_FRAC;
}

static inline int GetLumaYfromPtr(uint32* rgba) {
  const int r = GetRed(rgba);
  const int g = GetGreen(rgba);
  const int b = GetBlue(rgba);
  return GetLumaY(r, g, b);
}

static inline int GetChromaU(int r, int g, int b) {
  // U = -0.1483 * R - 0.2911 * G + 0.4394 * B + 128
  return clip_uv(-9719 * r - 19081 * g + 28800 * b);
}

static inline int GetChromaV(int r, int g, int b) {
  // V = 0.4394 * R - 0.3679 * G - 0.0715 * B + 128
  return clip_uv(+28800 * r - 24116 * g - 4684 * b);
}

/* Converts YUV to RGB and writes into a 32 bit pixel in endian
 * neutral fashion
 */
enum { RGB_FRAC = 16, RGB_HALF = (1 << RGB_FRAC) / 2,
       RGB_RANGE_MIN = -227, RGB_RANGE_MAX = 256 + 226 };

static int init_done = 0;
static int16_t kVToR[256], kUToB[256];
static int32_t kVToG[256], kUToG[256];
static uint8_t kClip[RGB_RANGE_MAX - RGB_RANGE_MIN];

static void InitTables() {
  int i;
  for (i = 0; i < 256; ++i) {
    kVToR[i] = (89858 * (i - 128) + RGB_HALF) >> RGB_FRAC;
    kUToG[i] = -22014 * (i - 128) + RGB_HALF;
    kVToG[i] = -45773 * (i - 128);
    kUToB[i] = (113618 * (i - 128) + RGB_HALF) >> RGB_FRAC;
  }
  for (i = RGB_RANGE_MIN; i < RGB_RANGE_MAX; ++i) {
    const int j = ((i - 16) * 76283 + RGB_HALF) >> RGB_FRAC;
    kClip[i - RGB_RANGE_MIN] = (j < 0) ? 0 : (j > 255) ? 255 : j;
  }

  init_done = 1;
}

static void ToRGB(int y, int u, int v, uint32* const dst) {
  const int r_off = kVToR[v];
  const int g_off = (kVToG[v] + kUToG[u]) >> RGB_FRAC;
  const int b_off = kUToB[u];
  const int r = kClip[y + r_off - RGB_RANGE_MIN];
  const int g = kClip[y + g_off - RGB_RANGE_MIN];
  const int b = kClip[y + b_off - RGB_RANGE_MIN];
  *dst = (r << RED_SHIFT) | (g << GREEN_SHIFT) | (b << BLUE_SHIFT);
}

static inline uint32 get_le32(const uint8* const data) {
  return data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}

/* Returns the difference (in dB) between two images represented in YUV format
 *
 * Input:
 *   Y1/U1/V1: The Y/U/V data of the first image
 *   Y2/U2/V2: The Y/U/V data of the second image
 *
 * Returns the PSNR (http://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio)
 * value computed between the two images
 */
double GetPSNRYuv(const uint8* Y1,
                  const uint8* U1,
                  const uint8* V1,
                  const uint8* Y2,
                  const uint8* U2,
                  const uint8* V2,
                  int y_width,
                  int y_height) {
  int x, y, row_idx;
  const int uv_width = ((y_width + 1) >> 1);
  const int uv_height = ((y_height + 1) >> 1);
  double sse = 0., count = 0.;
  for (y = 0; y < y_height; ++y) {
    count += y_width;
    row_idx = y * y_width;
    for (x = 0; x < y_width; ++x) {
      double diff = Y1[row_idx + x] - Y2[row_idx + x];
      sse += diff * diff;
    }
  }
  for (y = 0; y < uv_height; ++y) {
    count += 2 * uv_width;
    row_idx = y * uv_width;
    for (x = 0; x < uv_width; ++x) {
      const double diff_U = U1[row_idx + x] - U2[row_idx + x];
      const double diff_V = V1[row_idx + x] - V2[row_idx + x];
      sse += diff_U * diff_U + diff_V * diff_V;
    }
  }
  return -4.3429448 * log(sse / (255. * 255. * count));
}

/* Returns the difference (in dB) between two images. One represented
 * using Y,U,V vectors and the other is webp image data.
 * Input:
 *   Y1/U1/V1: The Y/U/V data of the first image
 *   imgdata: data buffer containing webp image
 *   imgdata_size: size of the imgdata buffer
 *
 * Returns the PSNR value computed between the two images
 */
double WebPGetPSNR(const uint8* Y1,
                   const uint8* U1,
                   const uint8* V1,
                   uint8* imgdata,
                   int imgdata_size) {
  uint8* Y2 = NULL;
  uint8* U2 = NULL;
  uint8* V2 = NULL;
  int w = 0, h = 0;
  double psnr = 0;

  WebPDecode(imgdata,
             imgdata_size,
             &Y2,
             &U2,
             &V2,
             &w,
             &h);

  psnr = GetPSNRYuv(Y1, U1, V1, Y2, U2, V2, w, h);
  free(Y2);

  return psnr;
}

/*---------------------------------------------------------------------*
 *                              Reading WebP                           *
 *---------------------------------------------------------------------*/

/* RIFF layout is:
 *   0ffset  tag
 *   0...3   "RIFF" 4-byte tag
 *   4...7   size of image data (including metadata) starting at offset 8
 *   8...11  "WEBP"  our form-type signature
 *   12..15  "VP8 " 4-byte tags, describing the raw video format used
 *   16..19  size of the raw WebP image data, starting at offset 20
 *   20....  the WebP bytes
 * There can be extra chunks after the "VP8 " chunk (ICMT, ICOP, ...)
 * All 32-bits sizes are in little-endian order.
 * Note: chunk data must be padded to multiple of 2 in size
 */

int SkipRiffHeader(const uint8** data_ptr, int *data_size_ptr) {
	/* 20 bytes RIFF header 10 bytes VP8 header */
	const int kHeaderSize = (20 + 10);
	uint32 chunk_size = 0xffffffffu;

	if (*data_size_ptr >= kHeaderSize && !memcmp(*data_ptr, "RIFF", 4)) {
	if (memcmp(*data_ptr + 8, "WEBP", 4)) {
		return 0;  /* wrong image file signature */
	} else {
		const uint32 riff_size = get_le32(*data_ptr + 4);
		if (memcmp(*data_ptr + 12, "VP8 ", 4)) {
		  return 0;   /* invalid compression format */
		}
		chunk_size = get_le32(*data_ptr + 16);
		if ((chunk_size > riff_size + 8) || (chunk_size & 1)) {
		  return 0;  /* inconsistent size information. */
		}
		/* We have a RIFF container. Skip it. */
		*data_ptr += 20;
		*data_size_ptr -= 20;
	}
	}
	return chunk_size;
}

/* Generate RGBA row from an YUV row (with width upsampling of chrome data)
 * Input:
 *    1, 2, 3. y_src, u_src, v_src - Pointers to input Y, U, V row data
 *    respectively. We reuse these variables, they iterate over all pixels in
 *    the row.
 *    4. y_width: width of the Y image plane (aka image width)
 * Output:
 *    5. rgb_sat: pointer to the output rgb row. We reuse this variable, it
 *    iterates over all pixels in the row.
 */
static void YUV420toRGBLine(uint8* y_src,
                            uint8* u_src,
                            uint8* v_src,
                            int y_width,
                            uint32* rgb_dst) {
  int x;
  for (x = 0; x < (y_width >> 1); ++x) {
    const int U = u_src[0];
    const int V = v_src[0];
    ToRGB(y_src[0], U, V, rgb_dst);
    ToRGB(y_src[1], U, V, rgb_dst + 1);
    ++u_src;
    ++v_src;
    y_src += 2;
    rgb_dst += 2;
  }
  if (y_width & 1) {      /* Rightmost pixel */
    ToRGB(y_src[0], (*u_src), (*v_src), rgb_dst);
  }
}

/* Converts from YUV (with color subsampling) such as produced by the WebPDecode
 * routine into 32 bits per pixel RGBA data array. This data array can be
 * directly used by the Leptonica Pix in-memory image format.
 * Input:
 *      1, 2, 3. Y, U, V: the input data buffers
 *      4. pixwpl: the desired words per line corresponding to the supplied
 *                 output pixdata.
 *      5. width, height: the dimensions of the image whose data resides in Y,
 *                        U, V.
 * Output:
 *     6. pixdata: the output data buffer. Caller should allocate
 *                 height * pixwpl bytes of memory before calling this routine.
 */
void YUV420toRGBA(uint8* Y,
                  uint8* U,
                  uint8* V,
                  int words_per_line,
                  int width,
                  int height,
                  uint32* pixdata) {
  int y_width = width;
  int y_stride = y_width;
  int uv_width = ((y_width + 1) >> 1);
  int uv_stride = uv_width;
  int y;

  if (!init_done)
    InitTables();

  /* note that the U, V upsampling in height is happening here as the U, V
   * buffers sent to successive odd-even pair of lines is same.
   */
  for (y = 0; y < height; ++y) {
    YUV420toRGBLine(Y + y * y_stride,
                    U + (y >> 1) * uv_stride,
                    V + (y >> 1) * uv_stride,
                    width,
                    pixdata + y * words_per_line);
  }
}

void gd_YUV420toRGBA(uint8* Y,
                  uint8* U,
                  uint8* V,
                  gdImagePtr im) {
  int width = im->sx;
  int height = im->sy;
  int y_width = width;
  int y_stride = y_width;
  int uv_width = ((y_width + 1) >> 1);
  int uv_stride = uv_width;
  int y;

  /* output im must be truecolor */
  if (!im->trueColor) {
  	return;
  }

  if (!init_done)
    InitTables();

  /* note that the U, V upsampling in height is happening here as the U, V
   * buffers sent to successive odd-even pair of lines is same.
   */
  for (y = 0; y < height; ++y) {
    YUV420toRGBLine(Y + y * y_stride,
                    U + (y >> 1) * uv_stride,
                    V + (y >> 1) * uv_stride,
                    width,
                    im->tpixels[y]);
  }
}

static WebPResult VPXDecode(const uint8* data,
                            int data_size,
                            uint8** p_Y,
                            uint8** p_U,
                            uint8** p_V,
                            int* p_width,
                            int* p_height) {
  vpx_codec_ctx_t dec;
  vp8_postproc_cfg_t ppcfg;
  WebPResult result = webp_failure;

  if (!data || data_size <= 10 || !p_Y || !p_U || !p_V
      || *p_Y != NULL || *p_U != NULL || *p_V != NULL) {
    return webp_failure;
  }

  if (vpx_codec_dec_init(&dec,
                         &vpx_codec_vp8_dx_algo, NULL, 0) != VPX_CODEC_OK) {
    return webp_failure;
  }

  ppcfg.post_proc_flag = VP8_NOFILTERING;
  vpx_codec_control(&dec, VP8_SET_POSTPROC, &ppcfg);


  if (vpx_codec_decode(&dec, data, data_size, NULL, 0) == VPX_CODEC_OK) {
    vpx_codec_iter_t iter = NULL;
    vpx_image_t* const img = vpx_codec_get_frame(&dec, &iter);
    if (img) {
      int y_width = img->d_w;
      int y_height = img->d_h;
      int y_stride = y_width;
      int uv_width = (y_width + 1) >> 1;
      int uv_stride = uv_width;
      int uv_height = ((y_height + 1) >> 1);
      int y;

      *p_width = y_width;
      *p_height = y_height;
      if ((*p_Y = (uint8 *)(calloc(y_stride * y_height
                                   + 2 * uv_stride * uv_height,
                                   sizeof(uint8)))) != NULL) {
        *p_U = *p_Y + y_height * y_stride;
        *p_V = *p_U + uv_height * uv_stride;
        for (y = 0; y < y_height; ++y) {
          memcpy(*p_Y + y * y_stride,
                 img->planes[0] + y * img->stride[0],
                 y_width);
        }
        for (y = 0; y < uv_height; ++y) {
          memcpy(*p_U + y * uv_stride,
                 img->planes[1] + y * img->stride[1],
                 uv_width);
          memcpy(*p_V + y * uv_stride,
                 img->planes[2] + y * img->stride[2],
                 uv_width);
        }
        result = webp_success;
      }
    }
  }
  vpx_codec_destroy(&dec);

  return result;
}

WebPResult WebPDecode(const uint8* data,
                      int data_size,
                      uint8** p_Y,
                      uint8** p_U,
                      uint8** p_V,
                      int* p_width,
                      int* p_height) {

  const uint32 chunk_size = SkipRiffHeader(&data, &data_size);
  if (!chunk_size) {
    return webp_failure; /* unsupported RIFF header */
  }

  return VPXDecode(data, data_size, p_Y, p_U, p_V, p_width, p_height);
}

/*---------------------------------------------------------------------*
 *                             Writing WebP                            *
 *---------------------------------------------------------------------*/

/* Takes a pair of RGBA row data as input and generates 2 rows of Y data and one
 * row of subsampled U, V data as output
 * Input:
 *    1, 2. rgb_line1, rgb_line2 - input rgba rows
 *    3. width - image width
 * Outout:
 *    4, 5, 6: Output Y, U, V row
 */
static void RGBALinepairToYUV420(uint32* rgb_line1,
                                 uint32* rgb_line2,
                                 int width,
                                 uint8* Y_dst1,
                                 uint8* Y_dst2,
                                 uint8* u_dst,
                                 uint8* v_dst) {
  int x;
  for (x = (width >> 1); x > 0; --x) {
    const int sum_r =
      GetRed(rgb_line1 + 0) + GetRed(rgb_line1 + 1) +
      GetRed(rgb_line2 + 0) + GetRed(rgb_line2 + 1);
    const int sum_g =
      GetGreen(rgb_line1 + 0) + GetGreen(rgb_line1 + 1) +
      GetGreen(rgb_line2 + 0) + GetGreen(rgb_line2 + 1);
    const int sum_b =
      GetBlue(rgb_line1 + 0) + GetBlue(rgb_line1 + 1) +
      GetBlue(rgb_line2 + 0) + GetBlue(rgb_line2 + 1);

    Y_dst1[0] = GetLumaYfromPtr(rgb_line1 + 0);
    Y_dst1[1] = GetLumaYfromPtr(rgb_line1 + 1);
    Y_dst2[0] = GetLumaYfromPtr(rgb_line2 + 0);
    Y_dst2[1] = GetLumaYfromPtr(rgb_line2 + 1);

    *u_dst++ = GetChromaU(sum_r, sum_g, sum_b);
    *v_dst++ = GetChromaV(sum_r, sum_g, sum_b);

    rgb_line1 += 2;
    rgb_line2 += 2;
    Y_dst1 += 2;
    Y_dst2 += 2;
  }

  if (width & 1) {    /* rightmost pixel. */
    const int sum_r = GetRed(rgb_line1) + GetRed(rgb_line2);
    const int sum_g = GetGreen(rgb_line1) + GetGreen(rgb_line2);
    const int sum_b = GetBlue(rgb_line1) + GetBlue(rgb_line2);

    Y_dst1[0] = GetLumaYfromPtr(rgb_line1);
    Y_dst2[0] = GetLumaYfromPtr(rgb_line2);
    *u_dst = GetChromaU(2 * sum_r, 2 * sum_g, 2 * sum_b);
    *v_dst = GetChromaV(2 * sum_r, 2 * sum_g, 2 * sum_b);
  }
}

/* Generates Y, U, V data (with color subsampling) from 32 bits
 * per pixel RGBA data buffer. The resulting YUV data can be directly fed into
 * the WebPEncode routine.
 * Input:
 *    1. pixdatainput rgba data buffer
 *    2. words per line corresponding to pixdata
 *    3, 4. image width and height respectively
 * Output:
 *    5, 6, 7. Output YUV data buffers
 */
void gd_RGBAToYUV420(gdImagePtr im2,
                  uint8* Y,
                  uint8* U,
                  uint8* V) {
  int y_width = im2->sx;
  int y_height = im2->sy;
  int y_stride = y_width;
  int uv_width = ((y_width + 1) >> 1);
  int uv_stride = uv_width;
  int y;
  gdImagePtr im = NULL;
  int free_im = 0;

  if (!im2->trueColor) {
  	/* Todo: Replace the color/YUV functions with our own and simplify
  	   that should boost the conversion a bit as well, not only for 
  	   palette image. */
  	im = gdImageCreateTrueColor(im2->sx, im2->sy);
  	if (!im) {
  		php_gd_error("gd-webp error: cannot convert palette input to truecolor");
  		return;
	}
  	gdImageCopy(im, im2, 0, 0, 0, 0, im->sx, im->sy);
  	free_im = 1;
  } else {
  	im = im2;
  }
  for (y = 0; y < (y_height >> 1); ++y) {
	RGBALinepairToYUV420(im->tpixels[2 * y],
						 im->tpixels[2 * y + 1],
						 y_width,
						 Y + 2 * y * y_stride,
						 Y + (2 * y + 1) * y_stride,
						 U + y * uv_stride,
						 V + y * uv_stride);
  }
  if (y_height & 1) {
	RGBALinepairToYUV420(im->tpixels[y_height - 1],
						 im->tpixels[y_height - 1],
						 y_width,
						 Y + (y_height - 1) * y_stride,
						 Y + (y_height - 1) * y_stride,
						 U + (y_height >> 1) * uv_stride,
						 V + (y_height >> 1) * uv_stride);
  }
	if (free_im) {
		gdImageDestroy(im);
	}
}

/* Generates Y, U, V data (with color subsampling) from 32 bits
 * per pixel RGBA data buffer. The resulting YUV data can be directly fed into
 * the WebPEncode routine.
 * Input:
 *    1. pixdatainput rgba data buffer
 *    2. words per line corresponding to pixdata
 *    3, 4. image width and height respectively
 * Output:
 *    5, 6, 7. Output YUV data buffers
 */
void RGBAToYUV420(uint32* pixdata,
                  int words_per_line,
                  int width,
                  int height,
                  uint8* Y,
                  uint8* U,
                  uint8* V) {
  int y_width = width;
  int y_height = height;
  int y_stride = y_width;
  int uv_width = ((y_width + 1) >> 1);
  int uv_stride = uv_width;
  int y;

  for (y = 0; y < (y_height >> 1); ++y) {
    RGBALinepairToYUV420(pixdata + 2 * y * words_per_line,
                         pixdata + (2 * y + 1) * words_per_line,
                         y_width,
                         Y + 2 * y * y_stride,
                         Y + (2 * y + 1) * y_stride,
                         U + y * uv_stride,
                         V + y * uv_stride);
  }
  if (y_height & 1) {
    RGBALinepairToYUV420(pixdata + (y_height - 1) * words_per_line,
                         pixdata + (y_height - 1) * words_per_line,
                         y_width,
                         Y + (y_height - 1) * y_stride,
                         Y + (y_height - 1) * y_stride,
                         U + (y_height >> 1) * uv_stride,
                         V + (y_height >> 1) * uv_stride);
  }
}

static int codec_ctl(vpx_codec_ctx_t *enc,
                     enum vp8e_enc_control_id id,
                     int value) {
  const vpx_codec_err_t res = vpx_codec_control_(enc, id, value);
  if (res != VPX_CODEC_OK) {
    return webp_failure;
  }
  return webp_success;
}

static void SetupParams(vpx_codec_enc_cfg_t* cfg,
                        int QP) {
  cfg->g_threads = 2;
  cfg->rc_min_quantizer = QP;
  cfg->rc_max_quantizer = QP;
  cfg->kf_mode = VPX_KF_FIXED;
}

/* VPXEncode: Takes a Y, U, V data buffers (with color components U and V
 *            subsampled to 1/2 resolution) and generates the VPX string.
 *            Output VPX string is placed in the *p_out buffer. container_size
 *            indicates number of bytes to be left blank at the beginning of
 *            *p_out buffer to accommodate for a container header.
 *
 * Return: success/failure
 */
static WebPResult VPXEncode(const uint8* Y,
                            const uint8* U,
                            const uint8* V,
                            int y_width,
                            int y_height,
                            int y_stride,
                            int uv_width,
                            int uv_height,
                            int uv_stride,
                            int QP,
                            int container_size,
                            unsigned char** p_out,
                            int* p_out_size_bytes) {
  vpx_codec_iface_t* iface = &vpx_codec_vp8_cx_algo;
  vpx_codec_err_t res;
  vpx_codec_enc_cfg_t cfg;
  vpx_codec_ctx_t enc;
  WebPResult result = webp_failure;
  vpx_image_t img;

  *p_out = NULL;
  *p_out_size_bytes = 0;


  /* validate input parameters. */
  if (!p_out || !Y || !U || !V
      || y_width <= 0 || y_height <= 0 || uv_width <= 0 || uv_height <= 0
      || y_stride < y_width || uv_stride < uv_width
      || QP < 0 || QP > 63) {
    return webp_failure;
  }

  res = vpx_codec_enc_config_default(iface, &cfg, 0);
  if (res != VPX_CODEC_OK) {
    return webp_failure;
  }

  SetupParams(&cfg, QP);
  cfg.g_w = y_width;
  cfg.g_h = y_height;

  res = vpx_codec_enc_init(&enc, iface, &cfg, 0);

  if (res == VPX_CODEC_OK) {
    codec_ctl(&enc, VP8E_SET_CPUUSED, 3);
    codec_ctl(&enc, VP8E_SET_NOISE_SENSITIVITY, 0);
    codec_ctl(&enc, VP8E_SET_SHARPNESS, 0);
    codec_ctl(&enc, VP8E_SET_ENABLEAUTOALTREF, 0);
    codec_ctl(&enc, VP8E_SET_ARNR_MAXFRAMES, 0);
    codec_ctl(&enc, VP8E_SET_ARNR_TYPE, 0);
    codec_ctl(&enc, VP8E_SET_ARNR_STRENGTH, 0);
    codec_ctl(&enc, VP8E_SET_STATIC_THRESHOLD, 0);
    codec_ctl(&enc, VP8E_SET_TOKEN_PARTITIONS, 2);

    vpx_img_wrap(&img, VPX_IMG_FMT_I420,
                 y_width, y_height, 16, (uint8*)(Y));
    img.planes[VPX_PLANE_Y] = (uint8*)(Y);
    img.planes[VPX_PLANE_U] = (uint8*)(U);
    img.planes[VPX_PLANE_V] = (uint8*)(V);
    img.stride[VPX_PLANE_Y] = y_stride;
    img.stride[VPX_PLANE_U] = uv_stride;
    img.stride[VPX_PLANE_V] = uv_stride;

    res = vpx_codec_encode(&enc, &img, 0, 1, 0, VPX_DL_BEST_QUALITY);

    if (res == VPX_CODEC_OK) {
      vpx_codec_iter_t iter = NULL;
      const vpx_codec_cx_pkt_t* pkt = vpx_codec_get_cx_data(&enc, &iter);
      if (pkt != NULL) {
        *p_out = (unsigned char*)(calloc(container_size + pkt->data.frame.sz,
                                         1));

        memcpy(*p_out + container_size,
               (const void*)(pkt->data.frame.buf),
               pkt->data.frame.sz);
        *p_out_size_bytes = container_size + pkt->data.frame.sz;

        result = webp_success;
      }
    }
  }

  vpx_codec_destroy(&enc);

  return result;
}

WebPResult WebPEncode(const uint8* Y,
                      const uint8* U,
                      const uint8* V,
                      int y_width,
                      int y_height,
                      int y_stride,
                      int uv_width,
                      int uv_height,
                      int uv_stride,
                      int QP,
                      unsigned char** p_out,
                      int* p_out_size_bytes,
                      double *psnr) {

  const int kRiffHeaderSize = 20;

  if (VPXEncode(Y, U, V,
                y_width, y_height, y_stride,
                uv_width, uv_height, uv_stride,
                QP, kRiffHeaderSize,
                p_out, p_out_size_bytes) != webp_success) {
    return webp_failure;
  } else {
	  /* Write RIFF header */
	  const int img_size_bytes  = *p_out_size_bytes - kRiffHeaderSize;
	  const int chunk_size = (img_size_bytes + 1) & ~1;  /* make size even */
	  const int riff_size = chunk_size + 12;
	  const uint8_t kRiffHeader[20] = { 'R', 'I', 'F', 'F',
										(riff_size >>  0) & 255,
										(riff_size >>  8) & 255,
										(riff_size >> 16) & 255,
										(riff_size >> 24) & 255,
										'W', 'E', 'B', 'P',
										'V', 'P', '8', ' ',
										(chunk_size >>  0) & 255,
										(chunk_size >>  8) & 255,
										(chunk_size >> 16) & 255,
										(chunk_size >> 24) & 255 };
	  memcpy(*p_out, kRiffHeader, kRiffHeaderSize);

	  if (img_size_bytes & 1) {  /* write a padding byte */
		const int new_size = *p_out_size_bytes + 1;
		unsigned char* p = (unsigned char*)realloc(*p_out, new_size);
		if (p == NULL) {
		  free(*p_out);
		  *p_out = NULL;
		  *p_out_size_bytes = 0;
		  return webp_failure;
		}
		p[new_size - 1] = 0;
        *p_out = p;
		*p_out_size_bytes = new_size;
	  }

	  if (psnr) {
		*psnr = WebPGetPSNR(Y, U, V, *p_out, *p_out_size_bytes);
	  }

	  return webp_success;
  }
}

void AdjustColorspace(uint8* Y, uint8* U, uint8* V, int width, int height) {
  int y_width = width;
  int y_height = height;
  int y_stride = y_width;
  int uv_width = ((y_width + 1) >> 1);
  int uv_height = ((y_height + 1) >> 1);
  int uv_stride = uv_width;
  int x, y;
  /* convert luma */
  for (y = 0; y < y_height; ++y) {
    uint8* const Yrow = Y + y * y_stride;
    for (x = 0; x < y_width; ++x) {
      /* maps [0..255] to [16..235] */
      Yrow[x] = ((Yrow[x] * 55 + 32) >> 6) + 16;
    }
  }
  /* convert chroma */
  for (y = 0; y < uv_height; ++y) {
    uint8* const Urow = U + y * uv_stride;
    uint8* const Vrow = V + y * uv_stride;
    for (x = 0; x < uv_width; ++x) {
      /* maps [0..255] to [16..240] */
      Urow[x] = (((Urow[x] - 127) * 7) >> 3) + 128;
      Vrow[x] = (((Vrow[x] - 127) * 7) >> 3) + 128;
    }
  }
}

void AdjustColorspaceBack(uint8* Y, uint8* U, uint8* V, int width, int height) {
  int y_width = width;
  int y_height = height;
  int y_stride = y_width;
  int uv_width = ((y_width + 1) >> 1);
  int uv_height = ((y_height + 1) >> 1);
  int uv_stride = uv_width;
  int x, y;
  /* convert luma */
  for (y = 0; y < y_height; ++y) {
    uint8* const Yrow = Y + y * y_stride;
    for (x = 0; x < y_width; ++x) {
      /* maps [16..235] to [0..255] */
      const int v = ((Yrow[x] - 16) * 149 + 64) >> 7;
      Yrow[x] = (v < 0) ? 0 : (v > 255) ? 255u : v;
    }
  }
  /* convert chroma */
  for (y = 0; y < uv_height; ++y) {
    uint8* const Urow = U + y * uv_stride;
    uint8* const Vrow = V + y * uv_stride;
    for (x = 0; x < uv_width; ++x) {
      /* maps [0..255] to [16..240] */
      const int ru = (((Urow[x] - 128) * 73) >> 6) + 128;
      const int rv = (((Vrow[x] - 128) * 73) >> 6) + 128;
      Urow[x] = (ru < 0) ? 0 : (ru > 255) ? 255u : ru;
      Vrow[x] = (rv < 0) ? 0 : (rv > 255) ? 255u : rv;
    }
  }
}

WebPResult WebPGetInfo(const uint8* data,
                       int data_size,
                       int *width,
                       int *height) {
	const uint32 chunk_size = SkipRiffHeader(&data, &data_size);

	if (width) *width = 0;
	if (height) *height = 0;

	if (!chunk_size) {
	return webp_failure; /* unsupported RIFF header */
	}

	/* Validate raw video data */
	if (data_size < 10) {
	return webp_failure;   /* not enough data */
	}

	/* check signature */
	if (data[3] != 0x9d || data[4] != 0x01 || data[5] != 0x2a) {
		return webp_failure;       /* Wrong signature. */
	} else {
		const uint32 bits = data[0] | (data[1] << 8) | (data[2] << 16);

		if ((bits & 1)) {   /* Not a keyframe. */
			return webp_failure;
		} else {
			const int profile = (bits >> 1) & 7;
			const int show_frame  = (bits >> 4) & 1;
			const uint32 partition_length = (bits >> 5);

			if (profile > 3) {
				return webp_failure;   /* unknown profile */
			}
			if (!show_frame) {
				return webp_failure;     /* first frame is invisible! */
			}
			if (partition_length >= chunk_size) {
				return webp_failure;   /* inconsistent size information. */
			} else {
				const int w = ((data[7] << 8) | data[6]) & 0x3fff;
				const int h = ((data[9] << 8) | data[8]) & 0x3fff;
				if (width) *width = w;
				if (height) *height = h;

				return webp_success;
			}
		}
	}
	return webp_failure;
}
#endif /* HAVE_LIBVPX */
