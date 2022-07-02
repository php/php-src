--TEST--
Bug 62523 (php crashes with segfault when exif_read_data called)
--EXTENSIONS--
exif
--FILE--
<?php
echo "Test\n";
var_dump(exif_read_data(__DIR__."/bug62523_3.jpg"));
?>
Done
--EXPECTF--
Test

Warning: exif_read_data(bug62523_3.jpg): File not supported in %sbug62523_3.php on line %d
bool(false)
Done
