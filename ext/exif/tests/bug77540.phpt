--TEST--
Bug 77540 (Invalid Read on exif_process_SOFn)
--EXTENSIONS--
exif
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