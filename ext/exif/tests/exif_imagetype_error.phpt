--TEST--
Test exif_imagetype() function : error conditions
--EXTENSIONS--
exif
--FILE--
<?php

echo "*** Testing exif_imagetype() : error conditions ***\n";

echo "\n-- Testing exif_imagetype() function with an unknown file  --\n";
var_dump( exif_imagetype(__DIR__.'/foo.jpg') );


?>
--EXPECTF--
*** Testing exif_imagetype() : error conditions ***

-- Testing exif_imagetype() function with an unknown file  --

Warning: exif_imagetype(%s/foo.jpg): Failed to open stream: No such file or directory in %s on line %d
bool(false)
