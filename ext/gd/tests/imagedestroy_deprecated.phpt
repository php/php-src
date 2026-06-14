--TEST--
imagedestroy() deprecation message
--EXTENSIONS--
gd
--FILE--
<?php
$img = imagecreate(5,5);
imagedestroy($img);
?>
--EXPECTF--
Deprecated: Function imagedestroy() is deprecated since 8.5, as it has no effect since PHP 8.0 in %s on line %d
