--TEST--
GH-20660 (imagefilleellipse() overflow)
--EXTENSIONS--
gd
--SKIPIF--
<?php if (getenv("SKIP_SLOW_TESTS")) die("skip slow test"); ?>
--FILE--
<?php
$im = imagecreate(8, 8);
imageellipse($im, 255, 255, "1234567890", 64, 0);
imagefilledellipse($im, 255, 255, "1234567890", 64, 0);
echo "OK";
?>
--EXPECT--
OK
