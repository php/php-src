--TEST--
image_type_to_extension()
--SKIPIF--
<?php
    require_once('skipif_imagetype.inc');
?>
--FILE--
<?php
    $constants = array(
        "IMAGETYPE_GIF"      => IMAGETYPE_GIF,
        "IMAGETYPE_JPEG"     => IMAGETYPE_JPEG,
        "IMAGETYPE_PNG"      => IMAGETYPE_PNG,
        "IMAGETYPE_SWF"      => IMAGETYPE_SWF,
        "IMAGETYPE_PSD"      => IMAGETYPE_PSD,
        "IMAGETYPE_BMP"      => IMAGETYPE_BMP,
        "IMAGETYPE_TIFF_II"  => IMAGETYPE_TIFF_II,
        "IMAGETYPE_TIFF_MM"  => IMAGETYPE_TIFF_MM,
        "IMAGETYPE_JPC"      => IMAGETYPE_JPC,
        "IMAGETYPE_JP2"      => IMAGETYPE_JP2,
        "IMAGETYPE_JPX"      => IMAGETYPE_JPX,
        "IMAGETYPE_JB2"      => IMAGETYPE_JB2,
        "IMAGETYPE_IFF"      => IMAGETYPE_IFF,
        "IMAGETYPE_WBMP"     => IMAGETYPE_WBMP,
        "IMAGETYPE_JPEG2000" => IMAGETYPE_JPEG2000,
        "IMAGETYPE_XBM"      => IMAGETYPE_XBM,
        "IMAGETYPE_WEBP"     => IMAGETYPE_WEBP,
        "IMAGETYPE_AVIF"     => IMAGETYPE_AVIF,
    );
    foreach($constants as $name => $constant) {
        printf("Constant: %s\n\tWith dot: %s\n\tWithout dot: %s\n", $name, image_type_to_extension($constant), image_type_to_extension($constant, false));
    }

    var_dump(image_type_to_extension(1000000, false));
    var_dump(image_type_to_extension(0));
?>
Done
--EXPECT--
Constant: IMAGETYPE_GIF
	With dot: .gif
	Without dot: gif
Constant: IMAGETYPE_JPEG
	With dot: .jpeg
	Without dot: jpeg
Constant: IMAGETYPE_PNG
	With dot: .png
	Without dot: png
Constant: IMAGETYPE_SWF
	With dot: .swf
	Without dot: swf
Constant: IMAGETYPE_PSD
	With dot: .psd
	Without dot: psd
Constant: IMAGETYPE_BMP
	With dot: .bmp
	Without dot: bmp
Constant: IMAGETYPE_TIFF_II
	With dot: .tiff
	Without dot: tiff
Constant: IMAGETYPE_TIFF_MM
	With dot: .tiff
	Without dot: tiff
Constant: IMAGETYPE_JPC
	With dot: .jpc
	Without dot: jpc
Constant: IMAGETYPE_JP2
	With dot: .jp2
	Without dot: jp2
Constant: IMAGETYPE_JPX
	With dot: .jpx
	Without dot: jpx
Constant: IMAGETYPE_JB2
	With dot: .jb2
	Without dot: jb2
Constant: IMAGETYPE_IFF
	With dot: .iff
	Without dot: iff
Constant: IMAGETYPE_WBMP
	With dot: .bmp
	Without dot: bmp
Constant: IMAGETYPE_JPEG2000
	With dot: .jpc
	Without dot: jpc
Constant: IMAGETYPE_XBM
	With dot: .xbm
	Without dot: xbm
Constant: IMAGETYPE_WEBP
	With dot: .webp
	Without dot: webp
Constant: IMAGETYPE_AVIF
	With dot: .avif
	Without dot: avif
bool(false)
bool(false)
Done
