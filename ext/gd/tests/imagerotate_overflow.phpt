--TEST--
imagerotate() overflow with negative numbers
--EXTENSIONS--
gd
--FILE--
<?php

$im = imagecreate(10, 10);

$tmp = imagerotate ($im, 5, -9999999);

var_dump($tmp);

?>
--EXPECT--
bool(false)
