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

#ifndef THIRD_PARTY_VP8_VP8IMG_H_
#define THIRD_PARTY_VP8_VP8IMG_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef enum WebPResultType {
  webp_success = 0,
  webp_failure = -1
} WebPResult;

/* Takes an array of bytes (string) corresponding to the WebP
 * encoded image and generates output in the YUV format with
 * the color components U, V subsampled to 1/2 resolution along
 * each dimension.
 * Input:
 *      1. data: the WebP data stream (array of bytes)
 *      2. data_size: count of bytes in the WebP data stream
 *
 * Output:
 *      3. p_Y/p_U/p_V : pointer to the Y/U/V data buffer (this routine will
 *                       allocate memory for the buffer, fill the buffer with
 *                       appropriate data and transfer owner ship of the buffer
 *                       to caller. Caller is reponsible for freeing the memory).
 *                       Note that the memory for Y, U, V buffers is alloacted
 *                       in one chunk, hence one should call free(*p_Y) only.
 *                       Do not try to free the U and V buffers.
 *
 *      6. p_width: this routine returns the width of the decoded image here
 *      7. p_height: this routine returns the width of the decoded image here
 * Return: success/failure
 */
WebPResult WebPDecode(const uint8* data,
                      int data_size,
                      uint8** p_Y,
                      uint8** p_U,
                      uint8** p_V,
                      int* p_width,
                      int* p_height);

/* WebPEncode: Takes a Y, U, V data buffers (with color components U and V
 *             subsampled to 1/2 resolution) and generates the WebP string.
 * Input:
 *      1, 2, 3. Y, U, V: The input YUV data buffers
 *      4, 5. y_width, y_height: The width and height of the image whose data
 *                               is in Y, U, V. This matches the Y plane. The U
 *                               and V planes typically have 1/2 width and
 *                               height.
 *      6. y_stride: The width (in bytes) of one row of Y data. This may not
 *                   match width if there is end of row padding (e.g., for 32
 *                   bit row alignment).
 *      7. QP: the quantization parameter. This parameter controls the
 *             compression vs quality tradeoff. Use smaller numbers for better
 *             quality (compression will be lesser) and vice versa. 20 is a
 *             good optimal value.
 * Output:
 *      8. p_out: the output array of bytes corresponding to the encoded WebP
 *                image. This routine allocates memory for the buffer, fills it
 *                with appropriate values and transfers ownership to caller.
 *                Caller responsible for freeing of memory.
 * Return: success/failure
 */
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
                      double* psnr);

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
                  uint32* pixdata);

/* Generates Y, U, V data (with color subsampling) from 32 bits
 * per pixel RGBA data buffer. The resulting YUV data can be directly fed into
 * the WebPEncode routine.
 * Input:
 *    1. pix data input rgba data buffer
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
                  uint8* V);

/* This function adjust from YUV420J (jpeg decoding) to YUV420 (webp input)
 * Hints: http://en.wikipedia.org/wiki/YCbCr
 */
void AdjustColorspace(uint8* Y, uint8* U, uint8* V, int width, int height);

/* Inverse function: convert from YUV420 to YUV420J */
void AdjustColorspaceBack(uint8* Y, uint8* U, uint8* V, int width, int height);

/* Checks WebP image header and outputs height and width information of
 * the image
 *
 * Input:
 *      1. data: the WebP data stream (array of bytes)
 *      2. data_size: count of bytes in the WebP data stream
 *
 * Outut:
 *      width/height: width and height of the image
 *
 * Return: success/failure
 */
WebPResult WebPGetInfo(const uint8* data,
                       int data_size,
                       int *width,
                       int *height);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* THIRD_PARTY_VP8_VP8IMG_H_ */
