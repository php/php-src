/* Convenience functions to read or write images from or to disk,
 * determining file type from the filename extension. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "gd.h"
#include "gd_intern.h"

typedef gdImagePtr(BGD_STDCALL *ReadFn)(FILE *in);
typedef void(BGD_STDCALL *WriteFn)(gdImagePtr im, FILE *out);
typedef gdImagePtr(BGD_STDCALL *LoadFn)(char *filename);

#ifdef HAVE_LIBZ
static void BGD_STDCALL writegd2(gdImagePtr im, FILE *out)
{
    gdImageGd2(im, out, 0, GD2_FMT_COMPRESSED);
} /* writegd*/
#endif

#ifdef HAVE_LIBJPEG
static void BGD_STDCALL writejpeg(gdImagePtr im, FILE *out)
{
    gdImageJpeg(im, out, -1);
} /* writejpeg*/
#endif

static void BGD_STDCALL writewbmp(gdImagePtr im, FILE *out)
{
    int fg = gdImageColorClosest(im, 0, 0, 0);

    gdImageWBMP(im, fg, out);
} /* writejpeg*/

static void BGD_STDCALL writebmp(gdImagePtr im, FILE *out)
{
    gdImageBmp(im, out, GD_TRUE);
} /* writejpeg*/

static const struct FileType {
    const char *ext;
    ReadFn reader;
    WriteFn writer;
    LoadFn loader;
} Types[] = {{".gif", gdImageCreateFromGif, gdImageGif, NULL},
             {".gd", gdImageCreateFromGd, gdImageGd, NULL},
             {".wbmp", gdImageCreateFromWBMP, writewbmp, NULL},
             {".bmp", gdImageCreateFromBmp, writebmp, NULL},

             {".xbm", gdImageCreateFromXbm, NULL, NULL},
             {".tga", gdImageCreateFromTga, NULL, NULL},

#ifdef HAVE_LIBAVIF
             {".avif", gdImageCreateFromAvif, gdImageAvif, NULL},
#endif

#ifdef HAVE_LIBPNG
             {".png", gdImageCreateFromPng, gdImagePng, NULL},
#endif

             {".qoi", gdImageCreateFromQoi, gdImageQoi, NULL},

#ifdef HAVE_LIBJPEG
             {".jpg", gdImageCreateFromJpeg, writejpeg, NULL},
             {".jpeg", gdImageCreateFromJpeg, writejpeg, NULL},
#endif

#ifdef HAVE_LIBHEIF
             {".heic", gdImageCreateFromHeif, gdImageHeif, NULL},
             {".heix", gdImageCreateFromHeif, NULL, NULL},
#endif

#ifdef HAVE_LIBTIFF
             {".tiff", gdImageCreateFromTiff, gdImageTiff, NULL},
             {".tif", gdImageCreateFromTiff, gdImageTiff, NULL},
#endif

#ifdef HAVE_LIBZ
             {".gd2", gdImageCreateFromGd2, writegd2, NULL},
#endif

#ifdef HAVE_LIBWEBP
             {".webp", gdImageCreateFromWebp, gdImageWebp, NULL},
#endif

#ifdef HAVE_LIBJXL
             {".jxl", gdImageCreateFromJxl, gdImageJxl, NULL},
#endif

#ifdef HAVE_LIBXPM
             {".xpm", NULL, NULL, gdImageCreateFromXpm},
#endif

             {NULL, NULL, NULL, NULL}};

static const struct FileType *ftype(const char *filename)
{
    int n;
    const char *ext;

    /* Find the file extension (i.e. the last period in the string. */
    ext = strrchr(filename, '.');
    if (!ext)
        return NULL;

    for (n = 0; Types[n].ext; n++) {
        if (gd_strcasecmp(ext, Types[n].ext) == 0) {
            return &Types[n];
        } /* if */
    } /* for */

    return NULL;
} /* ftype*/

/*
  Function: gdSupportsFileType

        Tests if a given file type is supported by GD.

        Given the name of an image file (which does not have to exist),
        returns 1 (i.e. TRUE) if <gdImageCreateFromFile> can read a file
        of that type.  This is useful if you do not know which image types
        were enabled at compile time.

        If _writing_ is true, the result will be true only if
        <gdImageFile> can write a file of this type.

        Note that filename parsing is done exactly the same as is done by
        <gdImageCreateFromFile> and <gdImageFile> and is subject to the
        same limitations.

        Assuming LibGD is compiled with support for these image types, the
        following extensions are supported:

                - .gif
                - .gd, .gd2
                - .wbmp
                - .bmp
                - .xbm
                - .tga
                - .png
                - .qoi
                - .jpg, .jpeg
                - .heif, .heix
                - .avif
                - .tiff, .tif
                - .webp
                - .jxl
                - .xpm

        Names are parsed case-insenstively.

  Parameters:

        filename    - Filename with tested extension.
        writing     - Flag: true tests if writing works

  Returns:

        GD_TRUE (1) if the file type is supported, GD_FALSE (0) if not.

*/
BGD_DECLARE(int)
gdSupportsFileType(const char *filename, int writing)
{
    const struct FileType *entry = ftype(filename);
    return !!entry && (!writing || !!entry->writer);
} /* gdSupportsFileType*/

/*
  Function: gdImageCreateFromFile

        Read an image file of any supported.

        Given the path to a file, <gdImageCreateFromFile> will open the
        file, read its contents with the appropriate _gdImageCreateFrom*_
        function and return it.

        File type is determined by the filename extension, so having an
        incorrect extension will probably not work.  For example, renaming
        PNG image "foo.png" to "foo.gif" and then attempting to load it
        will fail even if GD supports both formats.  See
        <gdSupportsFiletype> for more details.

        NULL is returned on error.

  Parameters:

        filename    - the input file name

  Returns:

        A pointer to the new image or NULL if an error occurred.

*/

BGD_DECLARE(gdImagePtr)
gdImageCreateFromFile(const char *filename)
{
    const struct FileType *entry = ftype(filename);
    FILE *fh;
    gdImagePtr result;

    if (!entry)
        return NULL;
    if (entry->loader)
        return entry->loader((char *)filename);
    if (!entry->reader)
        return NULL;

    fh = fopen(filename, "rb");
    if (!fh)
        return NULL;

    result = entry->reader(fh);

    fclose(fh);

    return result;
} /* gdImageCreateFromFile*/

/*
  Function: gdImageFile

        Writes an image to a file in the format indicated by the filename.

        File type is determined by the extension of the file name.  See
        <gdSupportsFiletype> for an overview of the parsing.

        For file types that require extra arguments, <gdImageFile>
        attempts to use sane defaults:

        <gdImageGd2>    - chunk size = 0, compression is enabled.
        <gdImageJpeg>   - quality = -1 (i.e. the reasonable default)
        <gdImageWBMP>   - foreground is the darkest available color

        Everything else is called with the two-argument function and so
        will use the default values.

        <gdImageFile> has some rudimentary error detection and will return
        GD_FALSE (0) if a detectable error occurred.  However, the image
        loaders do not normally return their error status so a result of
        GD_TRUE (1) does **not** mean the file was saved successfully.

  Parameters:

        im          - The image to save.
        filename    - The path to the file to which the image is saved.

  Returns:

        GD_FALSE (0) if an error was detected, GD_TRUE (1) if not.

*/

BGD_DECLARE(int)
gdImageFile(gdImagePtr im, const char *filename)
{
    const struct FileType *entry = ftype(filename);
    FILE *fh;

    if (!entry || !entry->writer)
        return GD_FALSE;

    fh = fopen(filename, "wb");
    if (!fh)
        return GD_FALSE;

    entry->writer(im, fh);

    fclose(fh);

    return GD_TRUE;
} /* gdImageFile*/
