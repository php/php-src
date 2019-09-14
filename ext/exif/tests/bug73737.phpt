--TEST--
Bug #73737 (Crash when parsing a tag format)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
	$exif = exif_thumbnail(__DIR__ . '/bug73737.tiff');
	var_dump($exif);
?>
--EXPECTF--
Warning: exif_thumbnail(bug73737.tiff): Error in TIFF: filesize(x0030) less than start of IFD dir(x10102) in %s line %d
bool(false)
