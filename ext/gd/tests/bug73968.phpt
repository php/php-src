--TEST--
Bug #73968 (Premature failing of XBM reading)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatefromxbm(__DIR__ . DIRECTORY_SEPARATOR . 'bug73968.xbm');
var_dump($im);
?>
--EXPECT--
object(GdImage)#1 (0) {
}
