--TEST--
GH-19904 (exif_read_data() reads EXIF metadata from WebP images)
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(__DIR__.'/gh19904.webp'));
?>
--EXPECTF--
array(26) {
  ["FileName"]=>
  string(12) "gh19904.webp"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(526)
  ["FileType"]=>
  int(18)
  ["MimeType"]=>
  string(10) "image/webp"
  ["SectionsFound"]=>
  string(24) "ANY_TAG, IFD0, EXIF, GPS"
  ["COMPUTED"]=>
  array(4) {
    ["IsColor"]=>
    int(0)
    ["ByteOrderMotorola"]=>
    int(0)
    ["UserComment"]=>
    string(17) "Created with GIMP"
    ["UserCommentEncoding"]=>
    string(9) "UNDEFINED"
  }
  ["ImageWidth"]=>
  int(100)
  ["ImageLength"]=>
  int(100)
  ["BitsPerSample"]=>
  array(3) {
    [0]=>
    int(8)
    [1]=>
    int(8)
    [2]=>
    int(8)
  }
  ["ImageDescription"]=>
  string(17) "Created with GIMP"
  ["XResolution"]=>
  string(5) "300/1"
  ["YResolution"]=>
  string(5) "300/1"
  ["ResolutionUnit"]=>
  int(2)
  ["Software"]=>
  string(10) "GIMP 3.0.4"
  ["DateTime"]=>
  string(19) "2025:09:21 15:30:30"
  ["Exif_IFD_Pointer"]=>
  int(250)
  ["GPS_IFD_Pointer"]=>
  int(430)
  ["DateTimeOriginal"]=>
  string(19) "2025:09:21 15:29:27"
  ["DateTimeDigitized"]=>
  string(19) "2025:09:21 15:29:27"
  ["OffsetTime"]=>
  string(6) "+02:00"
  ["OffsetTimeOriginal"]=>
  string(6) "+02:00"
  ["OffsetTimeDigitized"]=>
  string(6) "+02:00"
  ["UserComment"]=>
  string(25) "%sCreated with GIMP"
  ["ColorSpace"]=>
  int(1)
  ["GPSAltitude"]=>
  string(5) "0/100"
}
