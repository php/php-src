--TEST--
Bug #76409 (heap use after free in _php_stream_free)
--SKIPIF--
<?php
if (!extension_loaded('exif')) die('skip exif extension not available');
?>
--FILE--
<?php
exif_read_data('.');
?>
===DONE===
--EXPECTF--
Warning: exif_read_data(): Not a file in %s on line %d
===DONE===
