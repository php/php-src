--TEST--
Test max colors for a gd image.
--EXTENSIONS--
gd
--FILE--
<?php
imagecreatefromgd(__DIR__ . '/crafted.gd2');
?>
--EXPECTF--
Warning: imagecreatefromgd(): "%scrafted.gd2" is not a valid GD file in %s on line %d
