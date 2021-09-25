--TEST--
Bug #72482 (Ilegal write/read access caused by gdImageAALine overflow)
--EXTENSIONS--
gd
--FILE--
<?php
$img = imagecreatetruecolor(13, 1007);
imageantialias($img, true);
imageline($img, 0, 0, 1073745919, 1073745919, 4096);

$img = imagecreatetruecolor(100, 100);
imageantialias($img, true);
imageline($img, 1094795585, 0, 2147483647, 255, 0xff);
?>
===DONE===
--EXPECT--
===DONE===
