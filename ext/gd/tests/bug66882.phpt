--TEST--
Bug #66882 (imagerotate by -90 degrees truncates image by 1px)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagerotate(imagecreate(10, 10), -90, 0);
var_dump(imagesy($im), imagesx($im));
?>
--EXPECT--
int(10)
int(10)
