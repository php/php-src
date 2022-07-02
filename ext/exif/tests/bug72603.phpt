--TEST--
Bug #72603 (Out of bound read in exif_process_IFD_in_MAKERNOTE)
--EXTENSIONS--
exif
--FILE--
<?php
var_dump(count(exif_read_data(__DIR__ . "/bug72603.jpg")));
?>
--EXPECTF--
Warning: exif_read_data(bug72603.jpg): %s in %s%ebug72603.php on line %d
int(%d)
