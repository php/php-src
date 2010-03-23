--TEST--
Test image_type_to_mime_type() function : error conditions  - Pass incorrect number of arguments
--FILE--
<?php
/* Prototype  : proto string image_type_to_mime_type(int imagetype)
 * Description: Get Mime-Type for image-type returned by getimagesize, exif_read_data, exif_thumbnail, exif_imagetype 
 * Source code: ext/standard/image.c
 */

$imagetype = IMAGETYPE_GIF;
$extra_arg = 10;
echo "*** Testing image_type_to_mime_type() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing image_type_to_mime_type() function with Zero arguments --\n";
var_dump( image_type_to_mime_type() );

//Test image_type_to_mime_type with one more than the expected number of arguments
echo "\n-- Testing image_type_to_mime_type() function with more than expected no. of arguments --\n";
var_dump( image_type_to_mime_type($imagetype, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing image_type_to_mime_type() : error conditions ***

-- Testing image_type_to_mime_type() function with Zero arguments --

Warning: image_type_to_mime_type() expects exactly 1 parameter, 0 given in %simage_type_to_mime_type_error.php on line 13
NULL

-- Testing image_type_to_mime_type() function with more than expected no. of arguments --

Warning: image_type_to_mime_type() expects exactly 1 parameter, 2 given in %simage_type_to_mime_type_error.php on line 17
NULL
===DONE===