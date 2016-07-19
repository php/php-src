--TEST--
Bug 62523 (php crashes with segfault when exif_read_data called)
--SKIPIF--
<?php
extension_loaded("exif") or die("skip need exif");
?>
--FILE--
<?php
echo "Test\n";
var_dump(count(exif_read_data(__DIR__."/bug62523_2.jpg")));
?>
Done
--EXPECTF--
Test

Warning: exif_read_data(bug62523_2.jpg): IFD data bad offset: 0xADB23672 length 0x0D94 in %s%ebug62523_2.php on line %d
int(30)
Done
