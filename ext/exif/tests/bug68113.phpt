--TEST--
Bug #68113 (Heap corruption in exif_thumbnail())
--SKIPIF--
<?php
extension_loaded("exif") or die("skip need exif");
?>
--FILE--
<?php
var_dump(exif_thumbnail(__DIR__."/bug68113.jpg"));
?>
Done
--EXPECTF--
Warning: exif_thumbnail(bug68113.jpg): File structure corrupted in %s%ebug68113.php on line 2

Warning: exif_thumbnail(bug68113.jpg): Invalid JPEG file in %s%ebug68113.php on line 2
bool(false)
Done
