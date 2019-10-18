--TEST--
Bug #72094: Out of bounds heap read access in exif header processing
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
print_r(exif_read_data(__DIR__ . '/bug72094_1.jpg'));
print_r(exif_read_data(__DIR__ . '/bug72094_2.jpg'));
print_r(exif_read_data(__DIR__ . '/bug72094_3.jpg'));
print_r(exif_read_data(__DIR__ . '/bug72094_4.jpg'));
?>
DONE
--EXPECTF--
Warning: exif_read_data(bug72094_1.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_1.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_1.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_1.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_1.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_1.jpg): Process tag(x8298=Copyright): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_1.jpg): Illegal IFD offset in %sbug72094.php on line %d

Warning: exif_read_data(bug72094_1.jpg): File structure corrupted in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_1.jpg): Invalid JPEG file in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_2.jpg): Illegal IFD size in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_2.jpg): File structure corrupted in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_2.jpg): Invalid JPEG file in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_3.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_3.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_3.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_3.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_3.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_3.jpg): Process tag(x3030=UndefinedTag): Illegal format code 0x3030, suppose BYTE in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_3.jpg): Illegal IFD size in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_3.jpg): File structure corrupted in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_3.jpg): Invalid JPEG file in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_4.jpg): Invalid TIFF start (1) in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_4.jpg): File structure corrupted in %s%ebug72094.php on line %d

Warning: exif_read_data(bug72094_4.jpg): Invalid JPEG file in %s%ebug72094.php on line %d
DONE
