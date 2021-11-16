--TEST--
Bug #48378 (Infinite recursion due to corrupt JPEG)
--EXTENSIONS--
exif
--FILE--
<?php
exif_read_data(
__DIR__ . "/bug48378.jpg",
"FILE,COMPUTED,ANY_TAG"
);
?>
--EXPECTF--
Warning: exif_read_data(%s): Invalid IFD start in %s48378.php on line %d

Warning: exif_read_data(%s): Error reading from file: got=x08B4(=2228) != itemlen-2=x1FFE(=8190) in %s48378.php on line %d

Warning: exif_read_data(%s): Invalid JPEG file in %s48378.php on line %d
