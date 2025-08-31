--TEST--
Test image_type_to_mime_type() function : usage variations - Passing IMAGETYPE_ICO and IMAGETYPE_SWC
--EXTENSIONS--
zlib
--SKIPIF--
<?php
if (!defined("IMAGETYPE_SWC")) {
    die("skip IMAGETYPE_SWC is not defined ");
}
?>
--FILE--
<?php
echo "*** Testing image_type_to_mime_type() : usage variations ***\n";

error_reporting(E_ALL ^ E_NOTICE);

var_dump( image_type_to_mime_type(IMAGETYPE_ICO) );
var_dump( image_type_to_mime_type(IMAGETYPE_SWC) );
?>
--EXPECT--
*** Testing image_type_to_mime_type() : usage variations ***
string(24) "image/vnd.microsoft.icon"
string(29) "application/x-shockwave-flash"
