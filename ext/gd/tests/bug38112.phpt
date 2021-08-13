--TEST--
Bug #38112 (GIF Invalid Code size ).
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatefromgif(__DIR__ . '/bug38112.gif');
?>
--EXPECTF--
Warning: imagecreatefromgif(): "%sbug38112.gif" is not a valid GIF file in %sbug38112.php on line %d
