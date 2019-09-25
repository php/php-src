/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_COMPAT_H
#define PHP_COMPAT_H

#ifdef PHP_WIN32
#include "config.w32.h"
#else
#include <php_config.h>
#endif

#if defined(HAVE_BUNDLED_PCRE) || !defined(PHP_VERSION)
#define pcre2_jit_callback_8 php_pcre2_jit_callback
#define pcre2_callout_enumerate_8 php_pcre2_callout_enumerate
#define pcre2_code_copy_8 php_pcre2_code_copy
#define pcre2_code_copy_with_tables_8 php_pcre2_code_copy_with_tables
#define pcre2_code_free_8 php_pcre2_code_free
#define pcre2_compile_8 php_pcre2_compile
#define pcre2_compile_context_copy_8 php_pcre2_compile_context_copy
#define pcre2_compile_context_create_8 php_pcre2_compile_context_create
#define pcre2_compile_context_free_8 php_pcre2_compile_context_free
#define pcre2_config_8 php_pcre2_config
#define pcre2_convert_context_copy_8 php_pcre2_convert_context_copy
#define pcre2_convert_context_create_8 php_pcre2_convert_context_create
#define pcre2_convert_context_free_8 php_pcre2_convert_context_free
#define pcre2_dfa_match_8 php_pcre2_dfa_match
#define pcre2_general_context_copy_8 php_pcre2_general_context_copy
#define pcre2_general_context_create_8 php_pcre2_general_context_create
#define pcre2_general_context_free_8 php_pcre2_general_context_free
#define pcre2_get_error_message_8 php_pcre2_get_error_message
#define pcre2_get_mark_8 php_pcre2_get_mark
#define pcre2_get_ovector_pointer_8 php_pcre2_get_ovector_pointer
#define pcre2_get_ovector_count_8 php_pcre2_get_ovector_count
#define pcre2_get_startchar_8 php_pcre2_get_startchar
#define pcre2_jit_compile_8 php_pcre2_jit_compile
#define pcre2_jit_match_8 php_pcre2_jit_match
#define pcre2_jit_free_unused_memory_8 php_pcre2_jit_free_unused_memory
#define pcre2_jit_stack_assign_8 php_pcre2_jit_stack_assign
#define pcre2_jit_stack_create_8 php_pcre2_jit_stack_create
#define pcre2_jit_stack_free_8 php_pcre2_jit_stack_free
#define pcre2_maketables_8 php_pcre2_maketables
#define pcre2_match_8 php_pcre2_match
#define pcre2_match_context_copy_8 php_pcre2_match_context_copy
#define pcre2_match_context_create_8 php_pcre2_match_context_create
#define pcre2_match_context_free_8 php_pcre2_match_context_free
#define pcre2_match_data_create_8 php_pcre2_match_data_create
#define pcre2_match_data_create_from_pattern_8 php_pcre2_match_data_create_from_pattern
#define pcre2_match_data_free_8 php_pcre2_match_data_free
#define pcre2_pattern_info_8 php_pcre2_pattern_info
#define pcre2_serialize_decode_8 php_pcre2_serialize_decode
#define pcre2_serialize_encode_8 php_pcre2_serialize_encode
#define pcre2_serialize_free_8 php_pcre2_serialize_free
#define pcre2_serialize_get_number_of_codes_8 php_pcre2_serialize_get_number_of_codes
#define pcre2_set_bsr_8 php_pcre2_set_bsr
#define pcre2_set_callout_8 php_pcre2_set_callout
#define pcre2_set_character_tables_8 php_pcre2_set_character_tables
#define pcre2_set_compile_extra_options_8 php_pcre2_set_compile_extra_options
#define pcre2_set_compile_recursion_guard_8 php_pcre2_set_compile_recursion_guard
#define pcre2_set_depth_limit_8 php_pcre2_set_depth_limit
#define pcre2_set_glob_escape_8 php_pcre2_set_glob_escape
#define pcre2_set_glob_separator_8 php_pcre2_set_glob_separator
#define pcre2_set_heap_limit_8 php_pcre2_set_heap_limit
#define pcre2_set_match_limit_8 php_pcre2_set_match_limit
#define pcre2_set_max_pattern_length_8 php_pcre2_set_max_pattern_length
#define pcre2_set_newline_8 php_pcre2_set_newline
#define pcre2_set_parens_nest_limit_8 php_pcre2_set_parens_nest_limit
#define pcre2_set_offset_limit_8 php_pcre2_set_offset_limit
#define pcre2_substitute_8 php_pcre2_substitute
#define pcre2_substring_copy_byname_8 php_pcre2_substring_copy_byname
#define pcre2_substring_copy_bynumber_8 php_pcre2_substring_copy_bynumber
#define pcre2_substring_free_8 php_pcre2_substring_free
#define pcre2_substring_get_byname_8 php_pcre2_substring_get_byname
#define pcre2_substring_get_bynumber_8 php_pcre2_substring_get_bynumber
#define pcre2_substring_length_byname_8 php_pcre2_substring_length_byname
#define pcre2_substring_length_bynumber_8 php_pcre2_substring_length_bynumber
#define pcre2_substring_list_get_8 php_pcre2_substring_list_get
#define pcre2_substring_list_free_8 php_pcre2_substring_list_free
#define pcre2_substring_nametable_scan_8 php_pcre2_substring_nametable_scan
#define pcre2_substring_number_from_name_8 php_pcre2_substring_number_from_name
#define pcre2_set_recursion_limit_8 php_pcre2_set_recursion_limit
#define pcre2_set_recursion_memory_management_8 php_pcre2_set_recursion_memory_management
#endif

#define lookup				php_lookup
#define hashTableInit		php_hashTableInit
#define hashTableDestroy	php_hashTableDestroy
#define hashTableIterInit	php_hashTableIterInit
#define hashTableIterNext	php_hashTableIterNext

#if defined(HAVE_LIBXML) && (defined(HAVE_XML) || defined(HAVE_XMLRPC)) && !defined(HAVE_LIBEXPAT)
#define XML_DefaultCurrent php_XML_DefaultCurrent
#define XML_ErrorString php_XML_ErrorString
#define XML_ExpatVersion php_XML_ExpatVersion
#define XML_ExpatVersionInfo php_XML_ExpatVersionInfo
#define XML_ExternalEntityParserCreate php_XML_ExternalEntityParserCreate
#define XML_GetBase php_XML_GetBase
#define XML_GetBuffer php_XML_GetBuffer
#define XML_GetCurrentByteCount php_XML_GetCurrentByteCount
#define XML_GetCurrentByteIndex php_XML_GetCurrentByteIndex
#define XML_GetCurrentColumnNumber php_XML_GetCurrentColumnNumber
#define XML_GetCurrentLineNumber php_XML_GetCurrentLineNumber
#define XML_GetErrorCode php_XML_GetErrorCode
#define XML_GetIdAttributeIndex php_XML_GetIdAttributeIndex
#define XML_GetInputContext php_XML_GetInputContext
#define XML_GetSpecifiedAttributeCount php_XML_GetSpecifiedAttributeCount
#define XmlGetUtf16InternalEncodingNS php_XmlGetUtf16InternalEncodingNS
#define XmlGetUtf16InternalEncoding php_XmlGetUtf16InternalEncoding
#define XmlGetUtf8InternalEncodingNS php_XmlGetUtf8InternalEncodingNS
#define XmlGetUtf8InternalEncoding php_XmlGetUtf8InternalEncoding
#define XmlInitEncoding php_XmlInitEncoding
#define XmlInitEncodingNS php_XmlInitEncodingNS
#define XmlInitUnknownEncoding php_XmlInitUnknownEncoding
#define XmlInitUnknownEncodingNS php_XmlInitUnknownEncodingNS
#define XML_ParseBuffer php_XML_ParseBuffer
#define XML_Parse php_XML_Parse
#define XML_ParserCreate_MM php_XML_ParserCreate_MM
#define XML_ParserCreateNS php_XML_ParserCreateNS
#define XML_ParserCreate php_XML_ParserCreate
#define XML_ParserFree php_XML_ParserFree
#define XmlParseXmlDecl php_XmlParseXmlDecl
#define XmlParseXmlDeclNS php_XmlParseXmlDeclNS
#define XmlPrologStateInitExternalEntity php_XmlPrologStateInitExternalEntity
#define XmlPrologStateInit php_XmlPrologStateInit
#define XML_SetAttlistDeclHandler php_XML_SetAttlistDeclHandler
#define XML_SetBase php_XML_SetBase
#define XML_SetCdataSectionHandler php_XML_SetCdataSectionHandler
#define XML_SetCharacterDataHandler php_XML_SetCharacterDataHandler
#define XML_SetCommentHandler php_XML_SetCommentHandler
#define XML_SetDefaultHandlerExpand php_XML_SetDefaultHandlerExpand
#define XML_SetDefaultHandler php_XML_SetDefaultHandler
#define XML_SetDoctypeDeclHandler php_XML_SetDoctypeDeclHandler
#define XML_SetElementDeclHandler php_XML_SetElementDeclHandler
#define XML_SetElementHandler php_XML_SetElementHandler
#define XML_SetEncoding php_XML_SetEncoding
#define XML_SetEndCdataSectionHandler php_XML_SetEndCdataSectionHandler
#define XML_SetEndDoctypeDeclHandler php_XML_SetEndDoctypeDeclHandler
#define XML_SetEndElementHandler php_XML_SetEndElementHandler
#define XML_SetEndNamespaceDeclHandler php_XML_SetEndNamespaceDeclHandler
#define XML_SetEntityDeclHandler php_XML_SetEntityDeclHandler
#define XML_SetExternalEntityRefHandlerArg php_XML_SetExternalEntityRefHandlerArg
#define XML_SetExternalEntityRefHandler php_XML_SetExternalEntityRefHandler
#define XML_SetNamespaceDeclHandler php_XML_SetNamespaceDeclHandler
#define XML_SetNotationDeclHandler php_XML_SetNotationDeclHandler
#define XML_SetNotStandaloneHandler php_XML_SetNotStandaloneHandler
#define XML_SetParamEntityParsing php_XML_SetParamEntityParsing
#define XML_SetProcessingInstructionHandler php_XML_SetProcessingInstructionHandler
#define XML_SetReturnNSTriplet php_XML_SetReturnNSTriplet
#define XML_SetStartCdataSectionHandler php_XML_SetStartCdataSectionHandler
#define XML_SetStartDoctypeDeclHandler php_XML_SetStartDoctypeDeclHandler
#define XML_SetStartElementHandler php_XML_SetStartElementHandler
#define XML_SetStartNamespaceDeclHandler php_XML_SetStartNamespaceDeclHandler
#define XML_SetUnknownEncodingHandler php_XML_SetUnknownEncodingHandler
#define XML_SetUnparsedEntityDeclHandler php_XML_SetUnparsedEntityDeclHandler
#define XML_SetUserData php_XML_SetUserData
#define XML_SetXmlDeclHandler php_XML_SetXmlDeclHandler
#define XmlSizeOfUnknownEncoding php_XmlSizeOfUnknownEncoding
#define XML_UseParserAsHandlerArg php_XML_UseParserAsHandlerArg
#define XmlUtf16Encode php_XmlUtf16Encode
#define XmlUtf8Encode php_XmlUtf8Encode
#define XML_FreeContentModel php_XML_FreeContentModel
#define XML_MemMalloc php_XML_MemMalloc
#define XML_MemRealloc php_XML_MemRealloc
#define XML_MemFree php_XML_MemFree
#define XML_UseForeignDTD php_XML_UseForeignDTD
#define XML_GetFeatureList php_XML_GetFeatureList
#define XML_ParserReset php_XML_ParserReset

#ifdef HAVE_GD_BUNDLED
#define any2eucjp php_gd_any2eucjp
#define createwbmp php_gd_createwbmp
#define empty_output_buffer php_gd_empty_output_buffer
#define fill_input_buffer php_gd_fill_input_buffer
#define freewbmp php_gd_freewbmp
#define gdAlphaBlend php_gd_gdAlphaBlend
#define gdCompareInt php_gd_gdCompareInt
#define gdCosT php_gd_gdCosT
#define gdCtxPrintf php_gd_gdCtxPrintf
#define gdDPExtractData php_gd_gdDPExtractData
#define gdFontGetGiant php_gd_gdFontGetGiant
#define gdFontGetLarge php_gd_gdFontGetLarge
#define gdFontGetMediumBold php_gd_gdFontGetMediumBold
#define gdFontGetSmall php_gd_gdFontGetSmall
#define gdFontGetTiny php_gd_gdFontGetTiny
#define gdFontGiant php_gd_gdFontGiant
#define gdFontGiantData php_gd_gdFontGiantData
#define gdFontGiantRep php_gd_gdFontGiantRep
#define gdFontLarge php_gd_gdFontLarge
#define gdFontLargeData php_gd_gdFontLargeData
#define gdFontLargeRep php_gd_gdFontLargeRep
#define gdFontMediumBold php_gd_gdFontMediumBold
#define gdFontMediumBoldData php_gd_gdFontMediumBoldData
#define gdFontMediumBoldRep php_gd_gdFontMediumBoldRep
#define gdFontSmall php_gd_gdFontSmall
#define gdFontSmallData php_gd_gdFontSmallData
#define gdFontSmallRep php_gd_gdFontSmallRep
#define gdFontTiny php_gd_gdFontTiny
#define gdFontTinyData php_gd_gdFontTinyData
#define gdFontTinyRep php_gd_gdFontTinyRep
#define gdGetBuf php_gd_gdGetBuf
#define gdGetByte php_gd_gdGetByte
#define gdGetC php_gd_gdGetC
#define _gdGetColors php_gd__gdGetColors
#define gd_getin php_gd_gd_getin
#define gdGetInt php_gd_gdGetInt
#define gdGetWord php_gd_gdGetWord
#define gdImageAABlend php_gd_gdImageAABlend
#define gdImageAALine php_gd_gdImageAALine
#define gdImageAlphaBlending php_gd_gdImageAlphaBlending
#define gdImageAntialias php_gd_gdImageAntialias
#define gdImageArc php_gd_gdImageArc
#define gdImageBrightness php_gd_gdImageBrightness
#define gdImageChar php_gd_gdImageChar
#define gdImageCharUp php_gd_gdImageCharUp
#define gdImageColor php_gd_gdImageColor
#define gdImageColorAllocate php_gd_gdImageColorAllocate
#define gdImageColorAllocateAlpha php_gd_gdImageColorAllocateAlpha
#define gdImageColorClosest php_gd_gdImageColorClosest
#define gdImageColorClosestAlpha php_gd_gdImageColorClosestAlpha
#define gdImageColorClosestHWB php_gd_gdImageColorClosestHWB
#define gdImageColorDeallocate php_gd_gdImageColorDeallocate
#define gdImageColorExact php_gd_gdImageColorExact
#define gdImageColorExactAlpha php_gd_gdImageColorExactAlpha
#define gdImageColorMatch php_gd_gdImageColorMatch
#define gdImageColorResolve php_gd_gdImageColorResolve
#define gdImageColorResolveAlpha php_gd_gdImageColorResolveAlpha
#define gdImageColorTransparent php_gd_gdImageColorTransparent
#define gdImageCompare php_gd_gdImageCompare
#define gdImageContrast php_gd_gdImageContrast
#define gdImageConvolution php_gd_gdImageConvolution
#define gdImageCopy php_gd_gdImageCopy
#define gdImageCopyMerge php_gd_gdImageCopyMerge
#define gdImageCopyMergeGray php_gd_gdImageCopyMergeGray
#define gdImageCopyResampled php_gd_gdImageCopyResampled
#define gdImageCopyResized php_gd_gdImageCopyResized
#define gdImageCreate php_gd_gdImageCreate
#define gdImageCreateFromGd php_gd_gdImageCreateFromGd
#define gdImageCreateFromGd2 php_gd_gdImageCreateFromGd2
#define gdImageCreateFromGd2Ctx php_gd_gdImageCreateFromGd2Ctx
#define gdImageCreateFromGd2Part php_gd_gdImageCreateFromGd2Part
#define gdImageCreateFromGd2PartCtx php_gd_gdImageCreateFromGd2PartCtx
#define gdImageCreateFromGd2PartPtr php_gd_gdImageCreateFromGd2PartPtr
#define gdImageCreateFromGd2Ptr php_gd_gdImageCreateFromGd2Ptr
#define gdImageCreateFromGdCtx php_gd_gdImageCreateFromGdCtx
#define gdImageCreateFromGdPtr php_gd_gdImageCreateFromGdPtr
#define gdImageCreateFromGif php_gd_gdImageCreateFromGif
#define gdImageCreateFromGifCtx php_gd_gdImageCreateFromGifCtx
#define gdImageCreateFromGifSource php_gd_gdImageCreateFromGifSource
#define gdImageCreateFromJpeg php_gd_gdImageCreateFromJpeg
#define gdImageCreateFromJpegCtx php_gd_gdImageCreateFromJpegCtx
#define gdImageCreateFromJpegPtr php_gd_gdImageCreateFromJpegPtr
#define gdImageCreateFromPng php_gd_gdImageCreateFromPng
#define gdImageCreateFromPngCtx php_gd_gdImageCreateFromPngCtx
#define gdImageCreateFromPngPtr php_gd_gdImageCreateFromPngPtr
#define gdImageCreateFromPngSource php_gd_gdImageCreateFromPngSource
#define gdImageCreateFromWBMP php_gd_gdImageCreateFromWBMP
#define gdImageCreateFromWBMPCtx php_gd_gdImageCreateFromWBMPCtx
#define gdImageCreateFromWBMPPtr php_gd_gdImageCreateFromWBMPPtr
#define gdImageCreateFromXbm php_gd_gdImageCreateFromXbm
#define gdImageCreatePaletteFromTrueColor php_gd_gdImageCreatePaletteFromTrueColor
#define gdImageCreateTrueColor php_gd_gdImageCreateTrueColor
#define gdImageDashedLine php_gd_gdImageDashedLine
#define gdImageDestroy php_gd_gdImageDestroy
#define gdImageEdgeDetectQuick php_gd_gdImageEdgeDetectQuick
#define gdImageEllipse php_gd_gdImageEllipse
#define gdImageEmboss php_gd_gdImageEmboss
#define gdImageFill php_gd_gdImageFill
#define gdImageFilledArc php_gd_gdImageFilledArc
#define gdImageFilledEllipse php_gd_gdImageFilledEllipse
#define gdImageFilledPolygon php_gd_gdImageFilledPolygon
#define gdImageFilledRectangle php_gd_gdImageFilledRectangle
#define _gdImageFillTiled php_gd__gdImageFillTiled
#define gdImageFillToBorder php_gd_gdImageFillToBorder
#define gdImageGaussianBlur php_gd_gdImageGaussianBlur
#define gdImageGd php_gd_gdImageGd
#define gdImageGd2 php_gd_gdImageGd2
#define gdImageGd2Ptr php_gd_gdImageGd2Ptr
#define gdImageGdPtr php_gd_gdImageGdPtr
#define gdImageGetClip php_gd_gdImageGetClip
#define gdImageGetPixel php_gd_gdImageGetPixel
#define gdImageGetTrueColorPixel php_gd_gdImageGetTrueColorPixel
#define gdImageGif php_gd_gdImageGif
#define gdImageGifCtx php_gd_gdImageGifCtx
#define gdImageGifPtr php_gd_gdImageGifPtr
#define gdImageGrayScale php_gd_gdImageGrayScale
#define gdImageInterlace php_gd_gdImageInterlace
#define gdImageJpeg php_gd_gdImageJpeg
#define gdImageJpegCtx php_gd_gdImageJpegCtx
#define gdImageJpegPtr php_gd_gdImageJpegPtr
#define gdImageLine php_gd_gdImageLine
#define gdImageMeanRemoval php_gd_gdImageMeanRemoval
#define gdImageNegate php_gd_gdImageNegate
#define gdImagePaletteCopy php_gd_gdImagePaletteCopy
#define gdImagePng php_gd_gdImagePng
#define gdImagePngCtx php_gd_gdImagePngCtx
#define gdImagePngCtxEx php_gd_gdImagePngCtxEx
#define gdImagePngEx php_gd_gdImagePngEx
#define gdImagePngPtr php_gd_gdImagePngPtr
#define gdImagePngPtrEx php_gd_gdImagePngPtrEx
#define gdImagePngToSink php_gd_gdImagePngToSink
#define gdImagePolygon php_gd_gdImagePolygon
#define gdImageRectangle php_gd_gdImageRectangle
#define gdImageRotate php_gd_gdImageRotate
#define gdImageRotate180 php_gd_gdImageRotate180
#define gdImageRotate270 php_gd_gdImageRotate270
#define gdImageRotate45 php_gd_gdImageRotate45
#define gdImageRotate90 php_gd_gdImageRotate90
#define gdImageSaveAlpha php_gd_gdImageSaveAlpha
#define gdImageSelectiveBlur php_gd_gdImageSelectiveBlur
#define gdImageSetAntiAliased php_gd_gdImageSetAntiAliased
#define gdImageSetAntiAliasedDontBlend php_gd_gdImageSetAntiAliasedDontBlend
#define gdImageSetBrush php_gd_gdImageSetBrush
#define gdImageSetClip php_gd_gdImageSetClip
#define gdImageSetPixel php_gd_gdImageSetPixel
#define gdImageSetStyle php_gd_gdImageSetStyle
#define gdImageSetThickness php_gd_gdImageSetThickness
#define gdImageSetTile php_gd_gdImageSetTile
#define gdImageSkewX php_gd_gdImageSkewX
#define gdImageSkewY php_gd_gdImageSkewY
#define gdImageSmooth php_gd_gdImageSmooth
#define gdImageString php_gd_gdImageString
#define gdImageString16 php_gd_gdImageString16
#define gdImageStringFT php_gd_gdImageStringFT
#define gdImageStringFTEx php_gd_gdImageStringFTEx
#define gdImageStringTTF php_gd_gdImageStringTTF
#define gdImageStringUp php_gd_gdImageStringUp
#define gdImageStringUp16 php_gd_gdImageStringUp16
#define gdImageTrueColorToPalette php_gd_gdImageTrueColorToPalette
#define gdImageWBMP php_gd_gdImageWBMP
#define gdImageWBMPCtx php_gd_gdImageWBMPCtx
#define gdImageWBMPPtr php_gd_gdImageWBMPPtr
#define gdImageXbmCtx php_gd_gdImageXbmCtx
#define gdNewDynamicCtx php_gd_gdNewDynamicCtx
#define gdNewDynamicCtxEx php_gd_gdNewDynamicCtxEx
#define gdNewFileCtx php_gd_gdNewFileCtx
#define gdNewSSCtx php_gd_gdNewSSCtx
#define gdPutBuf php_gd_gdPutBuf
#define gdPutC php_gd_gdPutC
#define _gdPutColors php_gd__gdPutColors
#define gdPutInt php_gd_gdPutInt
#define gd_putout php_gd_gd_putout
#define gdPutWord php_gd_gdPutWord
#define gdSeek php_gd_gdSeek
#define gdSinT php_gd_gdSinT
#define gd_strtok_r php_gd_gd_strtok_r
#define gdTell php_gd_gdTell
#define getmbi php_gd_getmbi
#define init_destination php_gd_init_destination
#define init_source php_gd_init_source
#define jpeg_gdIOCtx_dest php_gd_jpeg_gdIOCtx_dest
#define jpeg_gdIOCtx_src php_gd_jpeg_gdIOCtx_src
#define lsqrt php_gd_lsqrt
#define printwbmp php_gd_printwbmp
#define Putchar php_gd_Putchar
#define putmbi php_gd_putmbi
#define Putword php_gd_Putword
#define readwbmp php_gd_readwbmp
#define skipheader php_gd_skipheader
#define skip_input_data php_gd_skip_input_data
#define term_destination php_gd_term_destination
#define term_source php_gd_term_source
#define writewbmp php_gd_writewbmp
#define ZeroDataBlock php_gd_ZeroDataBlock
#define gdCacheCreate php_gd_gdCacheCreate
#define gdCacheDelete php_gd_gdCacheDelete
#define gdCacheGet php_gd_gdCacheGet
#define gdFontCacheSetup php_gd_gdFontCacheSetup
#define gdFontCacheShutdown php_gd_gdFontCacheShutdown
#define gdFreeFontCache php_gd_gdFreeFontCache
#endif /* HAVE_GD_BUNDLED */

/* Define to specify how much context to retain around the current parse
   point. */
#define XML_CONTEXT_BYTES 1024

/* Define to make parameter entity parsing functionality available. */
#define XML_DTD 1

/* Define to make XML Namespaces functionality available. */
#define XML_NS 1
#endif

#ifdef PHP_EXPORTS
#define PCRE_STATIC
#endif

#endif
