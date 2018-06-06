--TEST--
Bug #73968 (Premature failing of XBM reading)
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip gd extension not available');
?>
--FILE--
<?php
$im = imagecreatefromxbm(__DIR__ . DIRECTORY_SEPARATOR . 'bug73968.xbm');
var_dump($im);
?>
===DONE===
--EXPECTF--
resource(%d) of type (gd)
===DONE===
