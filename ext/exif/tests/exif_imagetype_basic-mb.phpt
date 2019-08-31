--TEST--
Check for exif_imagetype default behaviour
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php

/* Prototype  : int exif_imagetype  ( string $filename  )
 * Description: Determine the type of an image
 * Source code: ext/exif/exif.c
*/
echo "*** Testing exif_imagetype() : basic functionality ***\n";

var_dump(exif_imagetype(__DIR__.'/test2私はガラスを食べられます.jpg'));
?>
===Done===
--EXPECT--
*** Testing exif_imagetype() : basic functionality ***
int(2)
===Done===
