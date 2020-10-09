--TEST--
Test exif_tagname() function : basic functionality
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php

echo "*** Testing exif_tagname() : basic functionality ***\n";

var_dump(exif_tagname(0x10E));
var_dump(exif_tagname(0x10F));
var_dump(exif_tagname(0x110));

?>
--EXPECT--
*** Testing exif_tagname() : basic functionality ***
string(16) "ImageDescription"
string(4) "Make"
string(5) "Model"
