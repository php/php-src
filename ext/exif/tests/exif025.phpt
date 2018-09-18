--TEST--
Check for exif_read_data, JPEG with IFD0, EXIF, INTEROP data in Motorola byte-order.
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(dirname(__FILE__).'/image025.jpg'));
?>
--EXPECTF--
array(14) {
  ["FileName"]=>
  string(12) "image025.jpg"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(%d)
  ["FileType"]=>
  int(2)
  ["MimeType"]=>
  string(10) "image/jpeg"
  ["SectionsFound"]=>
  string(28) "ANY_TAG, IFD0, EXIF, INTEROP"
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
  ["Exif_IFD_Pointer"]=>
  int(26)
  ["InteroperabilityOffset"]=>
  int(44)
  ["InterOperabilityIndex"]=>
  string(3) "R98"
  ["InterOperabilityVersion"]=>
  string(4) "0100"
  ["RelatedFileFormat"]=>
  string(12) "image025.jpg"
  ["RelatedImageWidth"]=>
  int(1)
  ["RelatedImageHeight"]=>
  int(1)
}
--CREDITS--
Eric Stewart <ericleestewart@gmail.com>
