--TEST--
Bug 76557 (heap-buffer-overflow (READ of size 48) while reading exif data)
--EXTENSIONS--
exif
--FILE--
<?php
var_dump(exif_read_data(__DIR__ . "/bug76557.jpg"));
?>
DONE
--EXPECTF--
Warning: exif_read_data(bug76557.jpg): Process tag(x010F=Make): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(): Further exif parsing errors have been suppressed in %s on line %d
bool(false)
DONE
