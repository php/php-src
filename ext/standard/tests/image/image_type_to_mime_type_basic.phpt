--TEST--
image_type_to_mime_type()
--FILE--
<?php

echo "Starting image_type_to_mime_type() test\n\n";

$image_types = array (
    IMAGETYPE_GIF,
    IMAGETYPE_JPEG,
    IMAGETYPE_PNG,
    IMAGETYPE_SWF,
    IMAGETYPE_PSD,
    IMAGETYPE_BMP,
    IMAGETYPE_TIFF_II,
    IMAGETYPE_TIFF_MM,
    IMAGETYPE_JPC,
    IMAGETYPE_JP2,
    IMAGETYPE_JPX,
    IMAGETYPE_JB2,
    IMAGETYPE_IFF,
    IMAGETYPE_WBMP,
    IMAGETYPE_JPEG2000,
    IMAGETYPE_HEIF,
    IMAGETYPE_XBM,
    IMAGETYPE_WEBP
);

    foreach($image_types as $image_type) {
        var_dump(image_type_to_mime_type($image_type));
    }

echo "\nDone image_type_to_mime_type() test\n";
?>
--EXPECT--
Starting image_type_to_mime_type() test

string(9) "image/gif"
string(10) "image/jpeg"
string(9) "image/png"
string(29) "application/x-shockwave-flash"
string(9) "image/psd"
string(9) "image/bmp"
string(10) "image/tiff"
string(10) "image/tiff"
string(24) "application/octet-stream"
string(9) "image/jp2"
string(24) "application/octet-stream"
string(24) "application/octet-stream"
string(9) "image/iff"
string(18) "image/vnd.wap.wbmp"
string(24) "application/octet-stream"
string(10) "image/heif"
string(9) "image/xbm"
string(10) "image/webp"

Done image_type_to_mime_type() test
