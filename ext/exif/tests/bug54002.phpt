--TEST--
Bug #54002 (crash on crafted tag)
--INI--
memory_limit=-1
--EXTENSIONS--
exif
--FILE--
<?php
exif_read_data(__DIR__ . '/bug54002_1.jpg');
exif_read_data(__DIR__ . '/bug54002_2.jpg');

?>
--EXPECTF--
Warning: exif_read_data(bug54002_1.jpg): Process tag(x0205=UndefinedTag): Illegal byte_count in %sbug54002.php on line %d

Warning: exif_read_data(bug54002_2.jpg): Process tag(x0205=UndefinedTag): Illegal byte_count in %sbug54002.php on line %d
