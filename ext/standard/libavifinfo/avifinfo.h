// Copyright (c) 2021, Alliance for Open Media. All rights reserved
//
// This source code is subject to the terms of the BSD 2 Clause License and
// the Alliance for Open Media Patent License 1.0. If the BSD 2 Clause License
// was not distributed with this source code in the LICENSE file, you can
// obtain it at www.aomedia.org/license/software. If the Alliance for Open
// Media Patent License 1.0 was not distributed with this source code in the
// PATENTS file, you can obtain it at www.aomedia.org/license/patent.

#ifndef AVIFINFO_H_
#define AVIFINFO_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------

typedef enum {
  kAvifInfoOk,             // The file was correctly parsed and the requested
                           // information was extracted. It is not guaranteed
                           // that the input bitstream is a valid complete
                           // AVIF file.
  kAvifInfoNotEnoughData,  // The input bitstream was correctly parsed until
                           // now but bytes are missing. The request should be
                           // repeated with more input bytes.
  kAvifInfoTooComplex,     // The input bitstream was correctly parsed until
                           // now but it is too complex. The parsing was
                           // stopped to avoid any timeout or crash.
  kAvifInfoInvalidFile,    // The input bitstream is not a valid AVIF file,
                           // truncated or not.
} AvifInfoStatus;

typedef struct {
  uint32_t width, height;   // In number of pixels. Ignores crop and rotation.
  uint32_t bit_depth;       // Likely 8, 10 or 12 bits per channel per pixel.
  uint32_t num_channels;    // Likely 1, 2, 3 or 4 channels:
                            //   (1 monochrome or 3 colors) + (0 or 1 alpha)
  uint8_t has_gainmap;      // True if a gain map was found.
  // Id of the gain map item. Assumes there is at most one. If there are several
  // gain map items (e.g. because the main image is tiled and each tile has an
  // independent gain map), then this is one of the ids, arbitrarily chosen.
  uint8_t gainmap_item_id;
  // Start location in bytes of the primary item id, relative to the beginning
  // of the given payload. The primary item id is a big endian number stored on
  // bytes primary_item_id_location to
  // primary_item_id_location+primary_item_id_bytes-1 inclusive.
  uint64_t primary_item_id_location;
  // Number of bytes of the primary item id.
  uint8_t primary_item_id_bytes;
} AvifInfoFeatures;

//------------------------------------------------------------------------------
// Fixed-size input API
// Use this API if a raw byte array of fixed size is available as input.

// Parses the 'data' and returns kAvifInfoOk if it is identified as an AVIF.
// 'data' can be partial but must point to the beginning of the AVIF file.
// The file type can be identified in the first 12 bytes of most AVIF files.
AvifInfoStatus AvifInfoIdentify(const uint8_t* data, size_t data_size);

// Parses the 'data' and returns kAvifInfoOk and its 'features' if it is
// identified as an AVIF file.
// 'data' can be partial but must point to the beginning of the AVIF file.
// The 'features' can be parsed in the first 450 bytes of most AVIF files.
// 'features' are set to 0 unless kAvifInfoOk is returned.
// There is no need to call AvifInfoIdentify() before AvifInfoGetFeatures().
// AvifInfoGetFeatures() parses the file further than AvifInfoIdentify() so it
// is possible that AvifInfoGetFeatures() returns errors while
// AvifInfoIdentify() returns kAvifInfoOk on the same given input bytes.
AvifInfoStatus AvifInfoGetFeatures(const uint8_t* data, size_t data_size,
                                   AvifInfoFeatures* features);

//------------------------------------------------------------------------------
// Streamed input API
// Use this API if the input bytes must be fetched and/or if the AVIF payload
// size is unknown. Implement the two function signatures below and pass them to
// AvifInfoRead*() with a 'stream', which can be anything (file, struct etc.).

// Reads 'num_bytes' from the 'stream'.
// The position in the 'stream' must be advanced by 'num_bytes'.
// Returns a pointer to the 'num_bytes' or null if it cannot be fulfilled.
// The returned data must remain valid until the next read.
typedef const uint8_t* (*read_stream_t)(void* stream, size_t num_bytes);
// Advances the position in the 'stream' by 'num_bytes'.
typedef void (*skip_stream_t)(void* stream, size_t num_bytes);

// Maximum number of bytes requested per read. There is no limit per skip.
#define AVIFINFO_MAX_NUM_READ_BYTES 64

// Same as AvifInfo*() but takes a 'stream' as input. AvifInfo*Stream() does
// not access the 'stream' directly but passes it as is to 'read' and 'skip'.
// 'read' cannot be null. If 'skip' is null, 'read' is called instead.
AvifInfoStatus AvifInfoIdentifyStream(void* stream, read_stream_t read,
                                      skip_stream_t skip);

// Can be called right after AvifInfoIdentifyStream() with the same 'stream'
// object if AvifInfoIdentifyStream() returned kAvifInfoOk.
// Any location-dependent feature such as 'primary_item_id_location' is relative
// to the given 'stream', and must be offset by the number of bytes read or
// skipped during AvifInfoIdentifyStream() if it was called prior to
// AvifInfoGetFeaturesStream() on the same 'stream' object.
// AvifInfoGetFeaturesStream() should only be called if AvifInfoIdentifyStream()
// returned kAvifInfoOk with the same input bytes.
AvifInfoStatus AvifInfoGetFeaturesStream(void* stream, read_stream_t read,
                                         skip_stream_t skip,
                                         AvifInfoFeatures* features);

//------------------------------------------------------------------------------

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // AVIFINFO_H_
