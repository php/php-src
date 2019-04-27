--TEST--
Bug #54002 (crash on crafted tag)
--INI--
memory_limit=-1
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
exif_read_data(__DIR__ . '/bug54002_1.jpeg');
exif_read_data(__DIR__ . '/bug54002_2.jpeg');

?>
--EXPECTF--
Warning: exif_read_data(bug54002_1.jpeg): Process tag(x0205=UndefinedTa): Illegal byte_count in %sbug54002.php on line %d

Warning: exif_read_data(bug54002_2.jpeg): Process tag(x0205=UndefinedTa): Illegal byte_count in %sbug54002.php on line %d
