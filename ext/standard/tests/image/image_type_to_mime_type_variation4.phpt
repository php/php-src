--TEST--
Test image_type_to_mime_type() function : usage variations - Passing IMAGETYPE_ICO and IMAGETYPE_SWC
--SKIPIF--
<?php
	if (!defined("IMAGETYPE_SWC") || !defined("IMAGETYPE_ICO") || !extension_loaded('zlib')) {
		die("skip zlib extension is not available or IMAGETYPE_SWC/IMAGETYPE_ICO is not defined ");
	}
?>
--FILE--
<?php
/* Prototype  : string image_type_to_mime_type(int imagetype)
 * Description: Get Mime-Type for image-type returned by getimagesize, exif_read_data, exif_thumbnail, exif_imagetype
 * Source code: ext/standard/image.c
 */


echo "*** Testing image_type_to_mime_type() : usage variations ***\n";

error_reporting(E_ALL ^ E_NOTICE);

var_dump( image_type_to_mime_type(IMAGETYPE_ICO) );
var_dump( image_type_to_mime_type(IMAGETYPE_SWC) );
?>
===DONE===
--EXPECT--
*** Testing image_type_to_mime_type() : usage variations ***
string(24) "image/vnd.microsoft.icon"
string(29) "application/x-shockwave-flash"
===DONE===
