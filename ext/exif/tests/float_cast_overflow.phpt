--TEST--
Overflow in float to int cast
--FILE--
<?php

var_dump(@exif_read_data(__DIR__ . '/float_cast_overflow.tiff'));

?>
--EXPECTF--
array(8) {
  ["FileName"]=>
  string(24) "float_cast_overflow.tiff"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(142)
  ["FileType"]=>
  int(7)
  ["MimeType"]=>
  string(10) "image/tiff"
  ["SectionsFound"]=>
  string(24) "ANY_TAG, IFD0, THUMBNAIL"
  ["COMPUTED"]=>
  array(5) {
    ["html"]=>
    string(20) "width="1" height="1""
    ["Height"]=>
    int(1)
    ["Width"]=>
    int(1)
    ["IsColor"]=>
    int(0)
    ["ByteOrderMotorola"]=>
    int(0)
  }
  ["THUMBNAIL"]=>
  array(2) {
    ["ImageWidth"]=>
    int(1)
    ["ImageLength"]=>
    float(-2.5961487387524E+33)
  }
}
