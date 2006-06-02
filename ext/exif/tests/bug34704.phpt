--TEST--
Bug #34704 (Infinite recursion due to corrupt JPEG)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--INI--
output_handler=
zlib.output_compression=0 
--FILE--
<?php
$infile = dirname(__FILE__).'/bug34704.jpg';
var_dump(exif_read_data($infile));
?>
===DONE===
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
===DONE===
--UEXPECTF--
array(7) {
  [u"FileName"]=>
  string(12) "bug34704.jpg"
  [u"FileDateTime"]=>
  int(1128867104)
  [u"FileSize"]=>
  int(9976)
  [u"FileType"]=>
  int(2)
  [u"MimeType"]=>
  string(10) "image/jpeg"
  [u"SectionsFound"]=>
  string(4) "IFD0"
  [u"COMPUTED"]=>
  array(5) {
    [u"html"]=>
    string(24) "width="386" height="488""
    [u"Height"]=>
    int(488)
    [u"Width"]=>
    int(386)
    [u"IsColor"]=>
    int(1)
    [u"ByteOrderMotorola"]=>
    int(0)
  }
}
===DONE===
