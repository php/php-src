--TEST--
Bug #68547 (Exif Header component value check error)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
exif_read_data(__DIR__ . DIRECTORY_SEPARATOR . 'bug68547.jpg');
?>
===DONE===
--EXPECTF--
Warning: exif_read_data(bug68547.jpg): Process tag(x9C9E=Keywords   ): Illegal components(%d) in %sbug68547.php on line %d
===DONE===
