--TEST--
Check for exif_read_data, unusual IFD start
--SKIPIF--
<?php 
	if (!extension_loaded('exif')) print 'skip exif extension not available';
?>
--FILE--
<?php
/* Do not change this test it is a REATME.TESTING example.
 * test5.jpg is a 1*1 image that contains an Exif section with ifd = 00000009h
 */
$image  = exif_read_data('./ext/exif/tests/test5.jpg','',true,false);
var_dump($image['IFD0']);
?>
--EXPECT--
array(2) {
  ["ImageDescription"]=>
  string(11) "Ifd00000009"
  ["DateTime"]=>
  string(19) "2002:10:18 20:06:00"
}