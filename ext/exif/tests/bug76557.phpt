--TEST--
Bug 76557 (heap-buffer-overflow (READ of size 48) while reading exif data)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
var_dump(exif_read_data(dirname(__FILE__) . "/bug76557.jpg"));
?>
DONE
--EXPECTF--
Warning: exif_read_data(bug76557.jpg): Process tag(x010F=Make       ): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x8769=Exif_IFD_Po): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x927C=MakerNote  ): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal format code 0x3030, suppose BYTE in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Process tag(x3030=UndefinedTa): Illegal pointer offset(x30303030 + x30303030 = x60606060 > x00EE) in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): File structure corrupted in %sbug76557.php on line %d

Warning: exif_read_data(bug76557.jpg): Invalid JPEG file in %sbug76557.php on line %d
bool(false)
DONE
