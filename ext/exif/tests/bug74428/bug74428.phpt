--TEST--
Bug #74428 (exif_read_data(): "Illegal IFD size" warning occurs with correct exif format)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
$infile = __DIR__.'/bug74428.jpg';
var_dump(exif_read_data($infile));
?>
===DONE===
--EXPECTF--
array(11) {
  ["FileName"]=>
  string(12) "bug74428.jpg"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(1902)
  ["FileType"]=>
  int(2)
  ["MimeType"]=>
  string(10) "image/jpeg"
  ["SectionsFound"]=>
  string(19) "ANY_TAG, IFD0, EXIF"
  ["COMPUTED"]=>
  array(5) {
    ["html"]=>
    string(22) "width="88" height="28""
    ["Height"]=>
    int(28)
    ["Width"]=>
    int(88)
    ["IsColor"]=>
    int(1)
    ["ByteOrderMotorola"]=>
    int(0)
  }
  ["Orientation"]=>
  int(1)
  ["Exif_IFD_Pointer"]=>
  int(38)
  ["ExifImageWidth"]=>
  int(88)
  ["ExifImageLength"]=>
  int(28)
}
===DONE===
