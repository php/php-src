--TEST--
Check for exif_read_data, Unicode WinXP tags	
--SKIPIF--
<?php 
	if (!extension_loaded('exif')) die('skip exif extension not available');
	if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
?>
--INI--
output_handler=
zlib.output_compression=0
exif.decode_unicode_intel=UCS-2LE
exif.decode_unicode_motorola=UCS-2BE
exif.encode_unicode=ISO-8859-1
--FILE--
<?php
/*
  test4.jpg is a 1*1 image that contains Exif tags written by WindowsXP
*/
$image  = exif_read_data('./ext/exif/tests/test4.jpg','',true,false);
echo var_dump($image['WINXP']);
?>
--EXPECT--
array(5) {
  ["Subject"]=>
  string(10) "Subject..."
  ["Keywords"]=>
  string(11) "Keywords..."
  ["Author"]=>
  string(9) "Rui Carmo"
  ["Comments"]=>
  string(29) "Comments
Line2
Line3
Line4"
  ["Title"]=>
  string(8) "Title..."
}
