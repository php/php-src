--TEST--
Bug #34704 (Infinite recursion due to corrupt JPEG)
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
$infile = __DIR__.'/bug34704.jpg';
var_dump(exif_read_data($infile));
?>
--EXPECTF--
array(7) {
  ["FileName"]=>
  string(12) "bug34704.jpg"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(9976)
  ["FileType"]=>
  int(2)
  ["MimeType"]=>
  string(10) "image/jpeg"
  ["SectionsFound"]=>
  string(4) "IFD0"
  ["COMPUTED"]=>
  array(5) {
    ["html"]=>
    string(24) "width="386" height="488""
    ["Height"]=>
    int(488)
    ["Width"]=>
    int(386)
    ["IsColor"]=>
    int(1)
    ["ByteOrderMotorola"]=>
    int(0)
  }
}
