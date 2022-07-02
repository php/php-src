--TEST--
Bug 77564 (Memory leak in exif_process_IFD_TAG)
--EXTENSIONS--
exif
--FILE--
<?php
var_dump(exif_read_data(__DIR__ . '/bug77564.jpg'));
?>
DONE
--EXPECTF--

Warning: exif_read_data(bug77564.jpg): Illegal IFD offset in %sbug77564.php on line %d

Warning: exif_read_data(bug77564.jpg): File structure corrupted in %sbug77564.php on line %d

Warning: exif_read_data(bug77564.jpg): Invalid JPEG file in %sbug77564.php on line %d
bool(false)
DONE
