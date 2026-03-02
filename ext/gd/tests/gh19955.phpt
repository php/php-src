--TEST--
GH-19955: (imagefttext() memory leak)
--EXTENSIONS--
gd
--CREDITS--
YuanchengJiang
--SKIPIF--
<?php
if(!function_exists('imagefttext')) die('skip imagefttext() not available');
?>
--FILE--
<?php
ini_set('error_reporting', E_ALL&~E_WARNING);
$im = imagecreate(64, 32);
imagefttext(imagecreate(8, 8), 0, 0, 0, 0, 255, __DIR__ . "/Tuffy.ttf", 'Ж');
echo "OK"
?>
--EXPECT--
OK
