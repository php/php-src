--TEST--
Check for exif_read_data, JPEG with IFD data containg OffsetTime, OffsetTimeOriginal, and OffsetTimeDigitized tags in Motorola byte-order.
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(__DIR__.'/image028.jpg'));
?>
--EXPECTF--
array(17) {
  ["FileName"]=>
  string(12) "image028.jpg"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(%d)
  ["FileType"]=>
  int(2)
  ["MimeType"]=>
  string(10) "image/jpeg"
  ["SectionsFound"]=>
  string(13) "ANY_TAG, IFD0"
  ["COMPUTED"]=>
  array(5) {
    ["html"]=>
    string(20) "width="1" height="1""
    ["Height"]=>
    int(1)
    ["Width"]=>
    int(1)
    ["IsColor"]=>
    int(1)
    ["ByteOrderMotorola"]=>
    int(1)
  }
  ["XResolution"]=>
  string(5) "300/1"
  ["YResolution"]=>
  string(5) "300/1"
  ["ResolutionUnit"]=>
  int(2)
  ["DateTime"]=>
  string(19) "2025:04:03 00:02:00"
  ["YCbCrPositioning"]=>
  int(1)
  ["DateTimeOriginal"]=>
  string(19) "2025:04:03 00:00:00"
  ["DateTimeDigitized"]=>
  string(19) "2025:04:03 00:01:00"
  ["OffsetTime"]=>
  string(6) "-02:00"
  ["OffsetTimeOriginal"]=>
  string(6) "+00:00"
  ["OffsetTimeDigitized"]=>
  string(6) "-01:00"
}
