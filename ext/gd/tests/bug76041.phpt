--TEST--
Bug #76041 (null pointer access crashed php)
--EXTENSIONS--
gd
--FILE--
<?php
$im=imagecreate(100, 100);
imageantialias($im, true);
imageline($im, 0, 0, 10, 10, 0xffffff);
?>
===DONE===
--EXPECT--
===DONE===
