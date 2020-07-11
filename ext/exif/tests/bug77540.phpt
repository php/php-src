--TEST--
Bug 77540 (Invalid Read on exif_process_SOFn)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
$width = $height = 42;
$s = exif_thumbnail(__DIR__."/bug77540.jpg", $width, $height);
echo "Width ".$width."\n";
echo "Height ".$height."\n";
?>
DONE
--EXPECT--
Width 0
Height 0
DONE