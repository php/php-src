--TEST--
Bug 62523 (php crashes with segfault when exif_read_data called)
--EXTENSIONS--
exif
--FILE--
<?php
echo "Test\n";
var_dump(count(exif_read_data(__DIR__."/bug62523_2.jpg")));
?>
Done
--EXPECT--
Test
int(72)
Done
