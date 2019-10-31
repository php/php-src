--TEST--
Test exif_imagetype() function : error conditions
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php

/* Prototype  : int exif_imagetype  ( string $filename  )
 * Description: Determine the type of an image
 * Source code: ext/exif/exif.c
*/

echo "*** Testing exif_imagetype() : error conditions ***\n";

echo "\n-- Testing exif_imagetype() function with an unknown file  --\n";
var_dump( exif_imagetype(__DIR__.'/foo.jpg') );


?>
--EXPECTF--
*** Testing exif_imagetype() : error conditions ***

-- Testing exif_imagetype() function with an unknown file  --

Warning: exif_imagetype(%s/foo.jpg): failed to open stream: No such file or directory in %s on line %d
bool(false)
