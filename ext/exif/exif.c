/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   +----------------------------------------------------------------------+
 */
/* Much of the code in this module was borrowed from the public domain
   jhead.c package with the author's consent.  The main changes have been
   to eliminate all the global variables to make it thread safe and to wrap
   it in the PHP 4 API.

   Aug.3 2001 - Added support for multiple M_COM entries - Rasmus
  
   The original header from the jhead.c file was:
  
  --------------------------------------------------------------------------
   Program to pull the information out of various types of EFIF digital 
   camera files and show it in a reasonably consistent way
  
   Version 0.9
  
   Compiles with MSVC on Windows, or with GCC on Linux

   Compileing under linux: Must include math library.
   Use: cc -lm -O3 -o jhead jhead.c
  
   Matthias Wandel,  Dec 1999 - April 2000
  --------------------------------------------------------------------------
  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_EXIF

#include "php_exif.h"
#include <math.h>
#include "php_ini.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"

typedef unsigned char uchar;

#ifndef TRUE
    #define TRUE 1
    #define FALSE 0
#endif

#define EXIF_MAX_COMMENTS 12

/* {{{ structs
   This structure stores Exif header image elements in a simple manner
   Used to store camera data as extracted from the various ways that it can be
   stored in a nexif header
*/
typedef struct {
    char  FileName     [120];
    time_t FileDateTime;
    unsigned FileSize;
    char  CameraMake   [32];
    char  CameraModel  [64];
    char  DateTime     [20];
    int   Height, Width;
    int   IsColor;
    int   FlashUsed;
    float FocalLength;
    float ExposureTime;
    float ApertureFNumber;
    float Distance;
    float CCDWidth;
    char  *Comments[EXIF_MAX_COMMENTS];
	int numComments;
	double FocalplaneXRes;
	double FocalplaneUnits;
	int ExifImageWidth;
	int MotorolaOrder;
	int Orientation;
	char GPSinfo[48];
	int ISOspeed;
	char ExifVersion[16];
	char Copyright[32];
	char Software[32];
	char *Thumbnail;
	int ThumbnailSize;
	int ThumbnailOffset;
	/* Olympus vars */
	int SpecialMode;
	int JpegQual;
	int Macro;
	int DigiZoom;
	char SoftwareRelease[16];
	char PictInfo[64];
	char CameraId[64];
	/* End Olympus vars */
} ImageInfoType;

/* This structure is used to store a section of a Jpeg file. */
typedef struct {
    uchar *Data;
    int      Type;
    unsigned Size;
} Section_t;
/* }}} */

#define EXIT_FAILURE  1
#define EXIT_SUCCESS  0

/* {{{ exif_functions[]
 */
function_entry exif_functions[] = {
    PHP_FE(read_exif_data, NULL)
    {NULL, NULL, NULL}
};
/* }}} */

PHP_MINFO_FUNCTION(exif);

/* {{{ exif_module_entry
 */
zend_module_entry exif_module_entry = {
    "exif",
    exif_functions,
    NULL, NULL,
    NULL, NULL,
    PHP_MINFO(exif),
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_EXIF
ZEND_GET_MODULE(exif)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(exif)
{
	php_info_print_table_start();
    php_info_print_table_row(2, "EXIF Support", "enabled" );
    php_info_print_table_end();
}
/* }}} */

/* {{{ Markers
   JPEG markers consist of one or more 0xFF bytes, followed by a marker
   code byte (which is not an FF).  Here are the marker codes of interest
   in this program.  (See jdmarker.c for a more complete list.)
*/

#define M_SOF0  0xC0            /* Start Of Frame N                        */
#define M_SOF1  0xC1            /* N indicates which compression process   */
#define M_SOF2  0xC2            /* Only SOF0-SOF2 are now in common use    */
#define M_SOF3  0xC3
#define M_SOF5  0xC5            /* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8            /* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9            /* End Of Image (end of datastream)         */
#define M_SOS   0xDA            /* Start Of Scan (begins compressed data)   */
#define M_JFIF  0xE0
#define M_EXIF  0xE1            
#define M_COM   0xFE            /* COMment                                  */


#define PSEUDO_IMAGE_MARKER 0x123; /* Extra value. */

/* }}} */

/* {{{ Get16m
   Get 16 bits motorola order (always) for jpeg header stuff.
*/
static int Get16m(void *Short)
{
    return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
}
/* }}} */

/* {{{ process_COM
   Process a COM marker.
   We want to print out the marker contents as legible text;
   we must guard against random junk and varying newline representations.
*/
static void process_COM (ImageInfoType *ImageInfo, uchar *Data, int length)
{
    int ch;
    char Comment[250];
    int nch;
    int a;

	if(ImageInfo->numComments == EXIF_MAX_COMMENTS) return;

    nch = 0;

    if (length > 200) length = 200; /* Truncate if it won't fit in our structure. */

    for (a=2;a<length;a++) {
        ch = Data[a];

        if (ch == '\r' && Data[a+1] == '\n') continue; /* Remove cr followed by lf. */

        if (isprint(ch) || ch == '\n' || ch == '\t') {
            Comment[nch++] = (char)ch;
        } else {
            Comment[nch++] = '?';
        }
    }

    Comment[nch] = '\0'; /* Null terminate */

	a = ImageInfo->numComments;
	
	(ImageInfo->Comments)[a] = emalloc(nch+1);
    strcpy(ImageInfo->Comments[a], Comment);
	(ImageInfo->numComments)++;
}
/* }}} */
 
/* {{{ process_SOFn
 * Process a SOFn marker.  This is useful for the image dimensions */
static void process_SOFn (ImageInfoType *ImageInfo, uchar *Data, int marker)
{
    int data_precision, num_components;
    const char *process;

    data_precision = Data[2];
    ImageInfo->Height = Get16m(Data+3);
    ImageInfo->Width = Get16m(Data+5);
    num_components = Data[7];

    if (num_components == 3) {
        ImageInfo->IsColor = 1;
    } else {
        ImageInfo->IsColor = 0;
    }

    switch (marker) {
        case M_SOF0:  process = "Baseline";  break;
        case M_SOF1:  process = "Extended sequential";  break;
        case M_SOF2:  process = "Progressive";  break;
        case M_SOF3:  process = "Lossless";  break;
        case M_SOF5:  process = "Differential sequential";  break;
        case M_SOF6:  process = "Differential progressive";  break;
        case M_SOF7:  process = "Differential lossless";  break;
        case M_SOF9:  process = "Extended sequential, arithmetic coding";  break;
        case M_SOF10: process = "Progressive, arithmetic coding";  break;
        case M_SOF11: process = "Lossless, arithmetic coding";  break;
        case M_SOF13: process = "Differential sequential, arithmetic coding";  break;
        case M_SOF14: process = "Differential progressive, arithmetic coding"; break;
        case M_SOF15: process = "Differential lossless, arithmetic coding";  break;
        default:      process = "Unknown";  break;
    }
}
/* }}} */

/* {{{ format description defines
   Describes format descriptor
*/
static int ExifBytesPerFormat[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};
#define NUM_FORMATS 12

#define FMT_BYTE       1 
#define FMT_STRING     2
#define FMT_USHORT     3
#define FMT_ULONG      4
#define FMT_URATIONAL  5
#define FMT_SBYTE      6
#define FMT_UNDEFINED  7
#define FMT_SSHORT     8
#define FMT_SLONG      9
#define FMT_SRATIONAL 10
#define FMT_SINGLE    11
#define FMT_DOUBLE    12

/*
   Describes tag values
*/

#define TAG_EXIF_OFFSET       0x8769
#define TAG_INTEROP_OFFSET    0xa005

#define TAG_COMPRESSION       0x0103

#define TAG_MAKE              0x010F
#define TAG_MODEL             0x0110
#define TAG_ORIENTATION       0x0112

#define TAG_SOFTWARE          0x0131

/* Olympus specific tags */
#define TAG_SPECIALMODE       0x0200
#define TAG_JPEGQUAL          0x0201
#define TAG_MACRO             0x0202
#define TAG_DIGIZOOM          0x0204
#define TAG_SOFTWARERELEASE   0x0207
#define TAG_PICTINFO          0x0208
#define TAG_CAMERAID          0x0209
/* end Olympus specific tags */

#define TAG_COPYRIGHT         0x8298

#define TAG_EXPOSURETIME      0x829A
#define TAG_FNUMBER           0x829D

#define TAG_GPSINFO           0x8825
#define TAG_ISOSPEED          0x8827
#define TAG_EXIFVERSION       0x9000

#define TAG_SHUTTERSPEED      0x9201
#define TAG_APERTURE          0x9202
#define TAG_MAXAPERTURE       0x9205
#define TAG_FOCALLENGTH       0x920A

#define TAG_DATETIME_ORIGINAL 0x9003
#define TAG_USERCOMMENT       0x9286

#define TAG_SUBJECT_DISTANCE  0x9206
#define TAG_LIGHT_SOURCE      0x9208
#define TAG_FLASH             0x9209

#define TAG_FOCALPLANEXRES    0xa20E
#define TAG_FOCALPLANEUNITS   0xa210
#define TAG_IMAGEWIDTH        0xA002
/* }}} */

/* {{{ TabTable[]
 */
static const struct {
    unsigned short Tag;
    char *Desc;
} TagTable[] = {
  {	0x100,	"ImageWidth"},
  {	0x101,	"ImageLength"},
  {	0x102,	"BitsPerSample"},
  {	0x103,	"Compression"},
  {	0x106,	"PhotometricInterpretation"},
  {	0x10A,	"FillOrder"},
  {	0x10D,	"DocumentName"},
  {	0x10E,	"ImageDescription"},
  {	0x10F,	"Make"},
  {	0x110,	"Model"},
  {	0x111,	"StripOffsets"},
  {	0x112,	"Orientation"},
  {	0x115,	"SamplesPerPixel"},
  {	0x116,	"RowsPerStrip"},
  {	0x117,	"StripByteCounts"},
  {	0x11A,	"XResolution"},
  {	0x11B,	"YResolution"},
  {	0x11C,	"PlanarConfiguration"},
  {	0x128,	"ResolutionUnit"},
  {	0x12D,	"TransferFunction"},
  {	0x131,	"Software"},
  {	0x132,	"DateTime"},
  {	0x13B,	"Artist"},
  {	0x13E,	"WhitePoint"},
  {	0x13F,	"PrimaryChromaticities"},
  {	0x156,	"TransferRange"},
  {	0x200,	"JPEGProc"},
  {	0x201,	"JPEGInterchangeFormat"},
  {	0x202,	"JPEGInterchangeFormatLength"},
  {	0x211,	"YCbCrCoefficients"},
  {	0x212,	"YCbCrSubSampling"},
  {	0x213,	"YCbCrPositioning"},
  {	0x214,	"ReferenceBlackWhite"},
  { 0x1000, "RelatedImageFileFormat"},
  {	0x828D,	"CFARepeatPatternDim"},
  {	0x828E,	"CFAPattern"},
  {	0x828F,	"BatteryLevel"},
  {	0x8298,	"Copyright"},
  {	0x829A,	"ExposureTime"},
  {	0x829D,	"FNumber"},
  {	0x83BB,	"IPTC/NAA"},
  {	0x8769,	"ExifOffset"},
  {	0x8773,	"InterColorProfile"},
  {	0x8822,	"ExposureProgram"},
  {	0x8824,	"SpectralSensitivity"},
  {	0x8825,	"GPSInfo"},
  {	0x8827,	"ISOSpeedRatings"},
  {	0x8828,	"OECF"},
  {	0x9000,	"ExifVersion"},
  {	0x9003,	"DateTimeOriginal"},
  {	0x9004,	"DateTimeDigitized"},
  {	0x9101,	"ComponentsConfiguration"},
  {	0x9102,	"CompressedBitsPerPixel"},
  {	0x9201,	"ShutterSpeedValue"},
  {	0x9202,	"ApertureValue"},
  {	0x9203,	"BrightnessValue"},
  {	0x9204,	"ExposureBiasValue"},
  {	0x9205,	"MaxApertureValue"},
  {	0x9206,	"SubjectDistance"},
  {	0x9207,	"MeteringMode"},
  {	0x9208,	"LightSource"},
  {	0x9209,	"Flash"},
  {	0x920A,	"FocalLength"},
  {	0x927C,	"MakerNote"},
  {	0x9286,	"UserComment"},
  {	0x9290,	"SubSecTime"},
  {	0x9291,	"SubSecTimeOriginal"},
  {	0x9292,	"SubSecTimeDigitized"},
  {	0xA000,	"FlashPixVersion"},
  {	0xA001,	"ColorSpace"},
  {	0xA002,	"ExifImageWidth"},
  {	0xA003,	"ExifImageLength"},
  {	0xA005,	"InteroperabilityOffset"},
  {	0xA20B,	"FlashEnergy"},			        /* 0x920B in TIFF/EP */
  {	0xA20C,	"SpatialFrequencyResponse"},	/* 0x920C    -  -    */
  {	0xA20E,	"FocalPlaneXResolution"},    	/* 0x920E    -  -    */
  {	0xA20F,	"FocalPlaneYResolution"},	    /* 0x920F    -  -    */
  {	0xA210,	"FocalPlaneResolutionUnit"},	/* 0x9210    -  -    */
  {	0xA214,	"SubjectLocation"},		        /* 0x9214    -  -    */
  {	0xA215,	"ExposureIndex"},		        /* 0x9215    -  -    */
  {	0xA217,	"SensingMethod"},		        /* 0x9217    -  -    */
  {	0xA300,	"FileSource"},
  {	0xA301,	"SceneType"},
  {      0, NULL}
} ;
/* }}} */

/* {{{ Get16u
 * Convert a 16 bit unsigned value from file's native byte order */
static int Get16u(void *Short, int MotorolaOrder)
{
    if (MotorolaOrder) {
        return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
    } else {
        return (((uchar *)Short)[1] << 8) | ((uchar *)Short)[0];
    }
}
/* }}} */

/* {{{ Get32s
 * Convert a 32 bit signed value from file's native byte order */
static int Get32s(void *Long, int MotorolaOrder)
{
    if (MotorolaOrder) {
        return  ((( char *)Long)[0] << 24) | (((uchar *)Long)[1] << 16)
              | (((uchar *)Long)[2] << 8 ) | (((uchar *)Long)[3] << 0 );
    } else {
        return  ((( char *)Long)[3] << 24) | (((uchar *)Long)[2] << 16)
              | (((uchar *)Long)[1] << 8 ) | (((uchar *)Long)[0] << 0 );
    }
}
/* }}} */

/* {{{ Get32u
 * Convert a 32 bit unsigned value from file's native byte order */
static unsigned Get32u(void *Long, int MotorolaOrder)
{
    return (unsigned)Get32s(Long, MotorolaOrder) & 0xffffffff;
}
/* }}} */

/* {{{ ConvertAnyFormat
 * Evaluate number, be it int, rational, or float from directory. */
static double ConvertAnyFormat(void *ValuePtr, int Format, int MotorolaOrder)
{
    double Value;
    Value = 0;

    switch(Format) {
        case FMT_SBYTE:     Value = *(signed char *)ValuePtr;  break;
        case FMT_BYTE:      Value = *(uchar *)ValuePtr;        break;

        case FMT_USHORT:    Value = Get16u(ValuePtr, MotorolaOrder);          break;
        case FMT_ULONG:     Value = Get32u(ValuePtr, MotorolaOrder);          break;

        case FMT_URATIONAL:
        case FMT_SRATIONAL: 
            {
                int Num, Den;
                Num = Get32s(ValuePtr, MotorolaOrder);
                Den = Get32s(4+(char *)ValuePtr, MotorolaOrder);
                if (Den == 0) {
                    Value = 0;
                } else {
                    Value = (double)Num/Den;
                }
                break;
            }

        case FMT_SSHORT:    Value = (signed short)Get16u(ValuePtr, MotorolaOrder);  break;
        case FMT_SLONG:     Value = Get32s(ValuePtr, MotorolaOrder);                break;

        /* Not sure if this is correct (never seen float used in Exif format) */
        case FMT_SINGLE:    Value = (double)*(float *)ValuePtr;      break;
        case FMT_DOUBLE:    Value = *(double *)ValuePtr;             break;
    }
    return Value;
}
/* }}} */

/* {{{ ExtractThumbnail
 * Grab the thumbnail - by Matt Bonneau */
static void ExtractThumbnail(ImageInfoType *ImageInfo, char *OffsetBase, unsigned ExifLength) {
	/* according to exif2.1, the thumbnail is not supposed to be greater than 64K */
	if (ImageInfo->ThumbnailSize > 65536) {
		php_error(E_WARNING, "Illegal thumbnail size");
		return;
	}

	ImageInfo->Thumbnail = emalloc(ImageInfo->ThumbnailSize);
	if (!ImageInfo->Thumbnail) {
		php_error(E_WARNING, "Could not allocate memory for thumbnail");
		return;
	} else {
		/* Check to make sure we are not going to go past the ExifLength */
		if ((unsigned)(ImageInfo->ThumbnailOffset + ImageInfo->ThumbnailSize) > ExifLength) {
			php_error(E_WARNING, "Thumbnail goes beyond exif header boundary");
			return;
		} else {
			memcpy(ImageInfo->Thumbnail, OffsetBase + ImageInfo->ThumbnailOffset, ImageInfo->ThumbnailSize);
		}
	}
}
/* }}} */

/* {{{ ProcessExifDir
 * Process one of the nested EXIF directories. */
static void ProcessExifDir(ImageInfoType *ImageInfo, char *DirStart, char *OffsetBase, unsigned ExifLength, char *LastExifRefd)
{
    int de;
    int a;
    int NumDirEntries;
    int NextDirOffset;
    

    NumDirEntries = Get16u(DirStart, ImageInfo->MotorolaOrder);

    if ((DirStart+2+NumDirEntries*12) > (OffsetBase+ExifLength)) {
        php_error(E_WARNING, "Illegally sized directory");
		return;
    }


	/*
    if (ShowTags) {
        printf("Directory with %d entries\n", NumDirEntries);
    }
	*/

    for (de=0;de<NumDirEntries;de++) {
        int Tag, Format, Components;
        char *ValuePtr;
        int ByteCount;
        char *DirEntry;
        DirEntry = DirStart+2+12*de;

        Tag = Get16u(DirEntry, ImageInfo->MotorolaOrder);
        Format = Get16u(DirEntry+2, ImageInfo->MotorolaOrder);
        Components = Get32u(DirEntry+4, ImageInfo->MotorolaOrder);

        if ((Format-1) >= NUM_FORMATS) {
            /* (-1) catches illegal zero case as unsigned underflows to positive large. */
            php_error(E_WARNING, "Illegal format code in EXIF dir");
			return;
        }

        ByteCount = Components * ExifBytesPerFormat[Format];

        if (ByteCount > 4) {
            unsigned OffsetVal;
            OffsetVal = Get32u(DirEntry+8, ImageInfo->MotorolaOrder);
            /* If its bigger than 4 bytes, the dir entry contains an offset. */
            if (OffsetVal+ByteCount > ExifLength) {
                /* Bogus pointer offset and / or bytecount value */
/*                printf("Offset %d bytes %d ExifLen %d\n", OffsetVal, ByteCount, ExifLength); */

                php_error(E_WARNING, "Illegal pointer offset value in EXIF");
				return;
            }
            ValuePtr = OffsetBase+OffsetVal;
        } else {
            /* 4 bytes or less and value is in the dir entry itself */
            ValuePtr = DirEntry+8;
        }

        if (LastExifRefd < ValuePtr+ByteCount) {
            /* 
			   Keep track of last byte in the exif header that was actually referenced.
               That way, we know where the discardable thumbnail data begins.
			*/
            LastExifRefd = ValuePtr+ByteCount;
        }

        /* Extract useful components of tag */
        switch(Tag) {

            case TAG_MAKE:
                strlcpy(ImageInfo->CameraMake, ValuePtr, sizeof(ImageInfo->CameraMake));
                break;

            case TAG_MODEL:
                strlcpy(ImageInfo->CameraModel, ValuePtr, sizeof(ImageInfo->CameraModel));
                break;

            case TAG_GPSINFO:
                strlcpy(ImageInfo->GPSinfo, ValuePtr, sizeof(ImageInfo->GPSinfo));
                break;

            case TAG_EXIFVERSION:
                strlcpy(ImageInfo->ExifVersion, ValuePtr, sizeof(ImageInfo->ExifVersion));
                break;

            case TAG_COPYRIGHT:
                strlcpy(ImageInfo->Copyright, ValuePtr, sizeof(ImageInfo->Copyright));
                break;

			case TAG_SOFTWARE:
                strlcpy(ImageInfo->Software, ValuePtr, sizeof(ImageInfo->Software));
                break;

			case TAG_ORIENTATION:
                ImageInfo->Orientation = (int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder);
				break;

			case TAG_ISOSPEED:
                ImageInfo->ISOspeed = (int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder);
				break;

            case TAG_DATETIME_ORIGINAL:
                strlcpy(ImageInfo->DateTime, ValuePtr, sizeof(ImageInfo->DateTime));
                break;

            case TAG_USERCOMMENT:
                /* Olympus has this padded with trailing spaces.  Remove these first. */
                for (a=ByteCount;;) {
                    a--;
                    if ((ValuePtr)[a] == ' ') {
                        (ValuePtr)[a] = '\0';
                    } else {
                        break;
                    }
                    if (a == 0) break;
                }

                /* Copy the comment */
                if (memcmp(ValuePtr, "ASCII", 5) == 0) {
                    for (a=5;a<10;a++) {
                        int c; int l;
                        c = (ValuePtr)[a];
                        if (c != '\0' && c != ' ') {
							l = strlen(a+ValuePtr)+1;
							l = (l<200)?l:201;
							(ImageInfo->Comments)[ImageInfo->numComments] = emalloc(l);
                            strlcpy((ImageInfo->Comments)[ImageInfo->numComments], a+ValuePtr, l);
							ImageInfo->numComments++;
                            break;
                        }
                    }
                    
                } else {
					int l;

					l = strlen(ValuePtr)+1;
					l = (l<200)?l:201;
					(ImageInfo->Comments)[ImageInfo->numComments] = emalloc(l);
                    strlcpy((ImageInfo->Comments)[ImageInfo->numComments], ValuePtr, l);
                }
                break;

            case TAG_FNUMBER:
                /* Simplest way of expressing aperture, so I trust it the most.
                   (overwrite previously computd value if there is one) */
                ImageInfo->ApertureFNumber = (float)ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder);
                break;

            case TAG_APERTURE:
            case TAG_MAXAPERTURE:
                /* More relevant info always comes earlier, so only use this field if we don't 
                   have appropriate aperture information yet. */
                if (ImageInfo->ApertureFNumber == 0) {
                    ImageInfo->ApertureFNumber 
                        = (float)exp(ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder)*log(2)*0.5);
                }
                break;

            case TAG_FOCALLENGTH:
                /* Nice digital cameras actually save the focal length as a function
                   of how farthey are zoomed in. */
                ImageInfo->FocalLength = (float)ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder);
                break;

            case TAG_SUBJECT_DISTANCE:
                /* Inidcates the distacne the autofocus camera is focused to.
                   Tends to be less accurate as distance increases. */
                ImageInfo->Distance = (float)ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder);
                break;

            case TAG_EXPOSURETIME:
                /* Simplest way of expressing exposure time, so I trust it most.
                   (overwrite previously computd value if there is one) */
                ImageInfo->ExposureTime = (float)ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder);
                break;

            case TAG_SHUTTERSPEED:
                /* More complicated way of expressing exposure time, so only use
                   this value if we don't already have it from somewhere else. */
                if (ImageInfo->ExposureTime == 0) {
                    ImageInfo->ExposureTime 
                        = (float)(1/exp(ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder)*log(2)));
                }
                break;

            case TAG_FLASH:
                if (ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder)) {
                    ImageInfo->FlashUsed = 1;
                }
                break;

            case TAG_IMAGEWIDTH:
                ImageInfo->ExifImageWidth = (int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder);
                break;

            case TAG_FOCALPLANEXRES:
                ImageInfo->FocalplaneXRes = ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder);
                break;

            case TAG_FOCALPLANEUNITS:
                switch((int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder)) {
                    case 1: ImageInfo->FocalplaneUnits = 25.4; break; /* inch */
                    case 2: 
                        /* According to the information I was using, 2 measn meters.
                           But looking at the Cannon powershot's files, inches is the only
                           sensible value. */
                        ImageInfo->FocalplaneUnits = 25.4;
                        break;

                    case 3: ImageInfo->FocalplaneUnits = 10;   break;  /* centimeter */
                    case 4: ImageInfo->FocalplaneUnits = 1;    break;  /* milimeter  */
                    case 5: ImageInfo->FocalplaneUnits = .001; break;  /* micrometer */
                }
                break;

            case TAG_LIGHT_SOURCE:
                /* Rarely set or useful. */
                break;

			case TAG_SPECIALMODE:
                ImageInfo->SpecialMode = (int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->SpecialMode);
				break;

			case TAG_JPEGQUAL: /* I think that this is a pointer to the thumbnail - let's see */
				ImageInfo->ThumbnailOffset = (int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->ThumbnailOffset);
				
				/* see if we know the size */
				if (ImageInfo->ThumbnailSize) {
					ExtractThumbnail(ImageInfo, OffsetBase, ExifLength);
				}
                /*ImageInfo->JpegQual = (int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->JpegQual);*/
				break;

			case TAG_MACRO: /* I think this is the size of the Thumbnail */
				ImageInfo->ThumbnailSize = (int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->ThumbnailSize);

				/* see if we have the offset */
				if (ImageInfo->ThumbnailOffset) {
					ExtractThumbnail(ImageInfo, OffsetBase, ExifLength);
				}
                /*ImageInfo->Macro = (int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->Macro);*/
				break;

			case TAG_DIGIZOOM:
                ImageInfo->DigiZoom = (int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->DigiZoom);
				break;

			case TAG_SOFTWARERELEASE:
                strlcpy(ImageInfo->SoftwareRelease, ValuePtr, sizeof(ImageInfo->SoftwareRelease));
				break;

			case TAG_PICTINFO:
                strlcpy(ImageInfo->PictInfo, ValuePtr, sizeof(ImageInfo->PictInfo));
				break;

			case TAG_CAMERAID:
                strlcpy(ImageInfo->CameraId, ValuePtr, sizeof(ImageInfo->CameraId));
				break;
        }

        if (Tag == TAG_EXIF_OFFSET || Tag == TAG_INTEROP_OFFSET) {
            char *SubdirStart;
            SubdirStart = OffsetBase + Get32u(ValuePtr, ImageInfo->MotorolaOrder);
            if (SubdirStart < OffsetBase || SubdirStart > OffsetBase+ExifLength) {
                php_error(E_WARNING, "Illegal subdirectory link");
				return;
            }
            ProcessExifDir(ImageInfo, SubdirStart, OffsetBase, ExifLength, LastExifRefd);
            continue;
        }
    }
    /*
     * Hack to make it process IDF1 I hope
     * There are 2 IDFs, the second one holds the keys (0x0201 and 0x0202) to the thumbnail
     */
    NextDirOffset = Get32u(DirStart+2+12*de, ImageInfo->MotorolaOrder);
    if (NextDirOffset) {
            if (OffsetBase + NextDirOffset < OffsetBase || OffsetBase + NextDirOffset > OffsetBase+ExifLength) {
                php_error(E_WARNING, "Illegal directory offset");
				return;
            }
	    ProcessExifDir(ImageInfo, OffsetBase + NextDirOffset, OffsetBase, ExifLength, LastExifRefd);
    }
}
/* }}} */

/* {{{ process_EXIF
   Process an EXIF marker
   Describes all the drivel that most digital cameras include...
*/
static void process_EXIF (ImageInfoType *ImageInfo, char *CharBuf, unsigned int length, char *LastExifRefd)
{
    ImageInfo->FlashUsed = 0; /* If it s from a digicam, and it used flash, it says so. */
    LastExifRefd = CharBuf;

    ImageInfo->FocalplaneXRes = 0;
    ImageInfo->FocalplaneUnits = 0;
    ImageInfo->ExifImageWidth = 0;

    /* set the thumbnail stuff to nothing so we can test to see if they get set up */
    ImageInfo->Thumbnail = NULL;
    ImageInfo->ThumbnailSize = 0;

    {   /* Check the EXIF header component */
        static const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
        if (memcmp(CharBuf+2, ExifHeader, 6)) {
            php_error(E_WARNING, "Incorrect Exif header");
			return;
        }
    }

    if (memcmp(CharBuf+8, "II", 2) == 0) {
/*        if (ShowTags) printf("Exif section in Intel order\n"); */
        ImageInfo->MotorolaOrder = 0;
    } else {
        if (memcmp(CharBuf+8, "MM", 2) == 0) {
/*            if (ShowTags) printf("Exif section in Motorola order\n"); */
            ImageInfo->MotorolaOrder = 1;
        } else {
            php_error(E_WARNING, "Invalid Exif alignment marker.");
			return;
        }
    }

    /* Check the next two values for correctness. */
    if (Get16u(CharBuf+10, ImageInfo->MotorolaOrder) != 0x2a
      || Get32u(CharBuf+12, ImageInfo->MotorolaOrder) != 0x08) {
        php_error(E_WARNING, "Invalid Exif start (1)");
		return;
    }

    /* First directory starts 16 bytes in.  Offsets start at 8 bytes in. */
    ProcessExifDir(ImageInfo, CharBuf+16, CharBuf+8, length-6, LastExifRefd);

    /* MB: This is where I will make my attempt to get the tumbnail */


    /* Compute the CCD width, in milimeters. */
    if (ImageInfo->FocalplaneXRes != 0) {
        ImageInfo->CCDWidth = (float)(ImageInfo->ExifImageWidth * ImageInfo->FocalplaneUnits / ImageInfo->FocalplaneXRes);
    }
}
/* }}} */

/* {{{ scan_JPEG_header
 * Parse the marker stream until SOS or EOI is seen; */
static int scan_JPEG_header (ImageInfoType *ImageInfo, FILE *infile, Section_t *Sections, int *SectionsRead, int ReadAll, char *LastExifRefd)
{
    int a;
/*
    int HaveCom = FALSE;
*/
    a = fgetc(infile);
    if (a != 0xff || fgetc(infile) != M_SOI) {
        return FALSE;
    }

    for(*SectionsRead=0;*SectionsRead < 19;) {
        int itemlen;
        int marker = 0;
        int ll, lh, got;
        uchar *Data;

        for (a=0;a<7;a++) {
            marker = fgetc(infile);
            if (marker != 0xff) break;
        }
        if (marker == 0xff) {
            /* 0xff is legal padding, but if we get that many, something's wrong. */
            php_error(E_WARNING, "too many padding bytes!");
			return FALSE;
        }

        Sections[*SectionsRead].Type = marker;
  
        /* Read the length of the section. */
        lh = fgetc(infile);
        ll = fgetc(infile);

        itemlen = (lh << 8) | ll;

        if (itemlen < 2) {
            php_error(E_WARNING, "invalid marker");
			return FALSE;
        }

        Sections[*SectionsRead].Size = itemlen;

        Data = (uchar *)emalloc(itemlen+1); /* Add 1 to allow sticking a 0 at the end. */
        Sections[*SectionsRead].Data = Data;

        /* Store first two pre-read bytes. */
        Data[0] = (uchar)lh;
        Data[1] = (uchar)ll;

        got = fread(Data+2, 1, itemlen-2, infile); /* Read the whole section. */
        if (got != itemlen-2) {
            php_error(E_WARNING, "reading from file");
			return FALSE;
        }
        *SectionsRead += 1;

        /*printf("Marker '%x' size %d\n", marker, itemlen);*/
        switch(marker) {
            case M_SOS:   /* stop before hitting compressed data  */
                /* If reading entire image is requested, read the rest of the data. */
                if (ReadAll) {
                    int cp, ep, size;
                    /* Determine how much file is left. */
                    cp = ftell(infile);
                    fseek(infile, 0, SEEK_END);
                    ep = ftell(infile);
                    fseek(infile, cp, SEEK_SET);

                    size = ep-cp;
                    Data = (uchar *)emalloc(size);
                    if (Data == NULL) {
                        php_error(E_WARNING, "could not allocate data for entire image");
						return FALSE;
                    }

                    got = fread(Data, 1, size, infile);
                    if (got != size) {
                        php_error(E_WARNING, "could not read the rest of the image");
						return FALSE;
                    }

                    Sections[*SectionsRead].Data = Data;
                    Sections[*SectionsRead].Size = size;
                    Sections[*SectionsRead].Type = PSEUDO_IMAGE_MARKER;
                    (*SectionsRead)++;
					/*
                    *HaveAll = 1;
					*/
					efree(Data);
                }
                return TRUE;

            case M_EOI:   /* in case it's a tables-only JPEG stream */
                php_error(E_WARNING, "No image in jpeg!");
                return FALSE;

            case M_COM: /* Comment section */
				/*
                if (HaveCom) {
                    (*SectionsRead) -= 1;
                    efree(Sections[*SectionsRead].Data);
                } else {
                    process_COM(ImageInfo, Data, itemlen);
                    HaveCom = TRUE;
                }
				*/
				process_COM(ImageInfo, Data, itemlen);
                break;

            case M_EXIF:
                if (*SectionsRead <= 2) {
                    /* Seen files from some 'U-lead' software with Vivitar scanner
                       that uses marker 31 later in the file (no clue what for!) */
                    process_EXIF(ImageInfo, (char *)Data, itemlen, LastExifRefd);
                }
                break;

			case M_JFIF:
				/* Regular jpegs always have this tag, exif images have the
				 * exif marker instead, althogh ACDsee will write images with
				 * both markers.  this program will re-create this marker on
				 * absence of exif marker.  Hence no need to keep the copy
				 * from the file. */
				efree(Sections[--(*SectionsRead)].Data);
				break;

            case M_SOF0: 
            case M_SOF1: 
            case M_SOF2: 
            case M_SOF3: 
            case M_SOF5: 
            case M_SOF6: 
            case M_SOF7: 
            case M_SOF9: 
            case M_SOF10:
            case M_SOF11:
            case M_SOF13:
            case M_SOF14:
            case M_SOF15:
                process_SOFn(ImageInfo, Data, marker);
                break;
            default:
                /* skip any other marker silently. */
                break;
        }
    }
    return TRUE;
}
/* }}} */

/* {{{ DiscardData
   Discard read data.
*/
void DiscardData(Section_t *Sections, int *SectionsRead)
{
    int a;
    for (a=0;a<*SectionsRead-1;a++) {
        efree(Sections[a].Data);
    }
    *SectionsRead = 0;
}
/* }}} */

/* {{{ ReadJpegFile
   Read image data.
*/
int ReadJpegFile(ImageInfoType *ImageInfo, Section_t *Sections, 
				  int *SectionsRead, char *FileName, 
				  int ReadAll, char *LastExifRefd TSRMLS_DC)
{
    FILE *infile;
    int ret;
	char *tmp;

    infile = VCWD_FOPEN(FileName, "rb"); /* Unix ignores 'b', windows needs it. */

    if (infile == NULL) {
        php_error(E_WARNING, "Unable to open '%s'", FileName);
        return FALSE;
    }
/*    CurrentFile = FileName; */

    /* Start with an empty image information structure. */
    memset(ImageInfo, 0, sizeof(*ImageInfo));
    memset(Sections, 0, sizeof(*Sections));

	tmp = php_basename(FileName, strlen(FileName), NULL, 0);
    strlcpy(ImageInfo->FileName, tmp, sizeof(ImageInfo->FileName));
	efree(tmp);
    ImageInfo->FocalLength = 0;
    ImageInfo->ExposureTime = 0;
    ImageInfo->ApertureFNumber = 0;
    ImageInfo->Distance = 0;
    ImageInfo->CCDWidth = 0;
    ImageInfo->FlashUsed = -1;
	ImageInfo->SpecialMode = -1;
	ImageInfo->JpegQual = -1;
	ImageInfo->Macro = -1;
	ImageInfo->DigiZoom = -1;

    {
        /* Store file date/time. */
        struct stat st;
        if (VCWD_STAT(FileName, &st) >= 0) {
            ImageInfo->FileDateTime = st.st_mtime;
            ImageInfo->FileSize = st.st_size;
        } else {
            php_error(E_WARNING, "Can't get file statitics");
			return FALSE;
        }
    }

    /* Scan the JPEG headers. */
    ret = scan_JPEG_header(ImageInfo, infile, Sections, SectionsRead, ReadAll, LastExifRefd);
    if (!ret) {
        php_error(E_WARNING, "Invalid Jpeg file: '%s'\n", FileName);
		return FALSE;
    }

    fclose(infile);

    return ret;
}
/* }}} */

/* {{{ php_read_jpeg_exif
 */
int php_read_jpeg_exif(ImageInfoType *ImageInfo, char *FileName, int ReadAll TSRMLS_DC)
{
	Section_t Sections[20];
	int SectionsRead;
	char *LastExifRefd=NULL;
	int ret;
	/* int i; */

	ImageInfo->MotorolaOrder = 0;

    ret = ReadJpegFile(ImageInfo, Sections, &SectionsRead, FileName, ReadAll, LastExifRefd TSRMLS_CC); 
	/*
	 * Thought this might pick out the embedded thumbnail, but it doesn't work.   -RL
    for (i=0;i<SectionsRead-1;i++) {
        if (Sections[i].Type == M_EXIF) {
			thumbsize = Sections[i].Size;
			if(thumbsize>0) {
				ImageInfo->Thumbnail = emalloc(thumbsize+5);
				ImageInfo->ThumbnailSize = thumbsize;
				ImageInfo->Thumbnail[0] = 0xff;
				ImageInfo->Thumbnail[1] = 0xd8;
				ImageInfo->Thumbnail[2] = 0xff;
				memcpy(ImageInfo->Thumbnail+4, Sections[i].Data, thumbsize+4);
			}
        }
    }
	*/
    if (ret != FALSE) {
        DiscardData(Sections, &SectionsRead);
    }
	return(ret);
}
/* }}} */

/* {{{ proto string read_exif_data(string filename [, int readall])
   Reads the EXIF header data from a JPEG file */ 
PHP_FUNCTION(read_exif_data) 
{
    pval **p_name, **p_readall, *tmpi;
    int ac = ZEND_NUM_ARGS(), ret, readall=1;
	ImageInfoType ImageInfo;
	char tmp[64];

    if ((ac < 1 || ac > 2) || zend_get_parameters_ex(ac, &p_name, &p_readall) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(p_name);

	if(ac == 2) {
		convert_to_long_ex(p_readall);
		readall = Z_LVAL_PP(p_readall);
	}

	ret = php_read_jpeg_exif(&ImageInfo, Z_STRVAL_PP(p_name), readall TSRMLS_CC);

	if (ret==FALSE || array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_assoc_string(return_value, "FileName", ImageInfo.FileName, 1);
	add_assoc_long(return_value, "FileDateTime", ImageInfo.FileDateTime);
	add_assoc_long(return_value, "FileSize", ImageInfo.FileSize);
	if (ImageInfo.CameraMake[0]) {
		add_assoc_string(return_value, "CameraMake", ImageInfo.CameraMake, 1);
	}
	if (ImageInfo.CameraModel[0]) {
		add_assoc_string(return_value, "CameraModel", ImageInfo.CameraModel, 1);
	}
	if (ImageInfo.DateTime[0]) {
		add_assoc_string(return_value, "DateTime", ImageInfo.DateTime, 1);
	}
	add_assoc_long(return_value, "Height", ImageInfo.Height);
	add_assoc_long(return_value, "Width", ImageInfo.Width);
	add_assoc_long(return_value, "IsColor", ImageInfo.IsColor);
	if(ImageInfo.FlashUsed >= 0) {
		add_assoc_long(return_value, "FlashUsed", ImageInfo.FlashUsed);
	}
	if (ImageInfo.FocalLength) {
		sprintf(tmp, "%4.1fmm", ImageInfo.FocalLength);
		add_assoc_string(return_value, "FocalLength", tmp, 1);
		if(ImageInfo.CCDWidth) {
			sprintf(tmp, "%dmm", (int)(ImageInfo.FocalLength/ImageInfo.CCDWidth*35+0.5));
			add_assoc_string(return_value, "35mmFocalLength", tmp, 1);
		}
		add_assoc_double(return_value, "RawFocalLength", ImageInfo.FocalLength);
	}
	if(ImageInfo.ExposureTime) {
		if(ImageInfo.ExposureTime <= 0.5) {
			sprintf(tmp, "%6.3f s (1/%d)", ImageInfo.ExposureTime, (int)(0.5 + 1/ImageInfo.ExposureTime));
		} else {
			sprintf(tmp, "%6.3f s", ImageInfo.ExposureTime);
		}	
		add_assoc_string(return_value, "ExposureTime", tmp, 1);
		add_assoc_double(return_value, "RawExposureTime", ImageInfo.ExposureTime);
	}
	if(ImageInfo.ApertureFNumber) {
		sprintf(tmp, "f/%4.1f", ImageInfo.ApertureFNumber);
		add_assoc_string(return_value, "ApertureFNumber", tmp, 1);
		add_assoc_double(return_value, "RawApertureFNumber", ImageInfo.ApertureFNumber);
	}
	if(ImageInfo.Distance) {
		if(ImageInfo.Distance<0) {
			add_assoc_string(return_value, "FocusDistance", "Infinite", 1);
		} else {
			sprintf(tmp, "%5.2fm", ImageInfo.Distance);
			add_assoc_string(return_value, "FocusDistance", tmp, 1);
		}
		add_assoc_double(return_value, "RawFocusDistance", ImageInfo.Distance);
	}
	if(ImageInfo.CCDWidth) {
		add_assoc_double(return_value, "CCDWidth", ImageInfo.CCDWidth);
	}
	if(ImageInfo.Orientation) {
		add_assoc_long(return_value, "Orientation", ImageInfo.Orientation);
	}
	if (ImageInfo.GPSinfo[0]) {
		add_assoc_string(return_value, "GPSinfo", ImageInfo.GPSinfo, 1);
	}
	if(ImageInfo.ISOspeed) {
		add_assoc_long(return_value, "ISOspeed", ImageInfo.ISOspeed);
	}
	if (ImageInfo.ExifVersion[0]) {
		add_assoc_string(return_value, "ExifVersion", ImageInfo.ExifVersion, 1);
	}
	if (ImageInfo.Copyright[0]) {
		add_assoc_string(return_value, "Copyright", ImageInfo.Copyright, 1);
	}
	if (ImageInfo.Software[0]) {
		add_assoc_string(return_value, "Software", ImageInfo.Software, 1);
	}
	if(ImageInfo.numComments) {
		if(ImageInfo.numComments==1) {
			add_assoc_string(return_value, "Comments", (ImageInfo.Comments)[0], 0);
		} else {
			int i;

			MAKE_STD_ZVAL(tmpi);
			array_init(tmpi);
			for(i=0; i<ImageInfo.numComments; i++) {
				add_index_string(tmpi, i, (ImageInfo.Comments)[i], 0);
			}
			zend_hash_update(return_value->value.ht, "Comments", 9, &tmpi, sizeof(zval *), NULL);
		}
	}
	if(ImageInfo.ThumbnailSize && ImageInfo.Thumbnail) {
		add_assoc_stringl(return_value, "Thumbnail", ImageInfo.Thumbnail, ImageInfo.ThumbnailSize, 1);
		add_assoc_long(return_value, "ThumbnailSize", ImageInfo.ThumbnailSize);
		efree(ImageInfo.Thumbnail);
	}
	if(ImageInfo.SpecialMode >= 0) {
		add_assoc_long(return_value, "SpecialMode", ImageInfo.SpecialMode);
	}
	if(ImageInfo.JpegQual >= 0) {
		add_assoc_long(return_value, "JpegQual", ImageInfo.JpegQual);
	}
	if(ImageInfo.Macro >= 0) {
		add_assoc_long(return_value, "Macro", ImageInfo.Macro);
	}
	if(ImageInfo.DigiZoom >= 0) {
		add_assoc_long(return_value, "DigiZoom", ImageInfo.DigiZoom);
	}
	if (ImageInfo.SoftwareRelease[0]) {
		add_assoc_string(return_value, "SoftwareRelease", ImageInfo.SoftwareRelease, 1);
	}
	if (ImageInfo.PictInfo[0]) {
		add_assoc_string(return_value, "PictInfo", ImageInfo.PictInfo, 1);
	}
	if (ImageInfo.CameraId[0]) {
		add_assoc_string(return_value, "CameraId", ImageInfo.CameraId, 1);
	}
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
