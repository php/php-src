--TEST--
Check for exif_imagetype default behaviour
--EXTENSIONS--
exif
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
