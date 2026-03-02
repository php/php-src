--TEST--
Bug #76409 (heap use after free in _php_stream_free)
--EXTENSIONS--
exif
--FILE--
<?php
exif_read_data('.');
?>
--EXPECTF--
Warning: exif_read_data(): %s in %s on line %d
