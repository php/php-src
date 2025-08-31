--TEST--
Bug #68113 (Heap corruption in exif_thumbnail())
--EXTENSIONS--
exif
--FILE--
<?php
var_dump(exif_thumbnail(__DIR__."/bug68113私はガラスを食べられます.jpg"));
?>
Done
--EXPECTF--
Warning: exif_thumbnail(bug68113私はガラスを食べられます.jpg): File structure corrupted in %s%ebug68113-mb.php on line 2

Warning: exif_thumbnail(bug68113私はガラスを食べられます.jpg): Invalid JPEG file in %s%ebug68113-mb.php on line 2
bool(false)
Done
