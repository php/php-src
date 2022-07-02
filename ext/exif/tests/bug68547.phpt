--TEST--
Bug #68547 (Exif Header component value check error)
--EXTENSIONS--
mbstring
exif
--FILE--
<?php
var_dump(exif_read_data(__DIR__ . DIRECTORY_SEPARATOR . 'bug68547.jpg'));
?>
--EXPECTF--
array(12) {
  ["FileName"]=>
  string(12) "bug68547.jpg"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(713)
  ["FileType"]=>
  int(2)
  ["MimeType"]=>
  string(10) "image/jpeg"
  ["SectionsFound"]=>
  string(20) "ANY_TAG, IFD0, WINXP"
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
    int(0)
  }
  ["Subject"]=>
  string(10) "Subjec??.."
  ["Keywords"]=>
  string(0) ""
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
