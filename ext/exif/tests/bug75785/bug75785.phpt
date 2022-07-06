--TEST--
Bug #75785 fix corrupt EXIF header issues; Related to mixed endianness. (Thank you @Richard Matzinger for providing the test photo)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
$mixedEndiannessFile = dirname(__FILE__).'/P1000506.JPG';
$tags = exif_read_data($mixedEndiannessFile, 'EXIF', true, false);

echo $tags['GPS']['GPSLatitude'][0] . PHP_EOL;
echo $tags['GPS']['GPSLongitude'][0] . PHP_EOL;
?>
===DONE===
--EXPECTF--
38/1
122/1
===DONE===
