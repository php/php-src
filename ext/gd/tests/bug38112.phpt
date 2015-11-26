--TEST--
Bug #38112 (GIF Invalid Code size ).
--SKIPIF--
<?php
	if (!extension_loaded('gd')) {
		die("skip gd extension not available\n");
	}
?>
--FILE--
<?php
$im = imagecreatefromgif(dirname(__FILE__) . '/bug38112.gif');
?>
--EXPECTF--
Warning: imagecreatefromgif(): '%sbug38112.gif' is not a valid GIF file in %sbug38112.php on line %d
