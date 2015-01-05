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

echo "\n-- Testing exif_imagetype() function with no arguments --\n";
var_dump( exif_imagetype() );

echo "\n-- Testing exif_imagetype() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( exif_imagetype(dirname(__FILE__).'/test2.jpg', $extra_arg) );

echo "\n-- Testing exif_imagetype() function with an unknown file  --\n";
var_dump( exif_imagetype(dirname(__FILE__).'/foo.jpg') );


?>
===Done===
--EXPECTF--
*** Testing exif_imagetype() : error conditions ***

-- Testing exif_imagetype() function with no arguments --

Warning: exif_imagetype() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing exif_imagetype() function with more than expected no. of arguments --

Warning: exif_imagetype() expects exactly 1 parameter, 2 given in %s on line %d
NULL

-- Testing exif_imagetype() function with an unknown file  --

Warning: exif_imagetype(%s/foo.jpg): failed to open stream: No such file or directory in %s on line %d
bool(false)
===Done===

