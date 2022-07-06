--TEST--
Check for exif_imagetype default behaviour
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php

echo "*** Testing exif_imagetype() : basic functionality ***\n";

var_dump(exif_imagetype(__DIR__.'/test2私はガラスを食べられます.jpg'));
?>
--EXPECT--
*** Testing exif_imagetype() : basic functionality ***
int(2)
