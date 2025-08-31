--TEST--
Check for exif_read_data, baseline JPEG with no IFD, EXIF, GPS or Interoperability data in Intel byte-order.
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(__DIR__.'/image007.jpg'));
?>
--EXPECTF--
array(7) {
  ["FileName"]=>
  string(12) "image007.jpg"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(%d)
  ["FileType"]=>
  int(2)
  ["MimeType"]=>
  string(10) "image/jpeg"
  ["SectionsFound"]=>
  string(0) ""
  ["COMPUTED"]=>
  array(4) {
    ["html"]=>
    string(20) "width="1" height="1""
    ["Height"]=>
    int(1)
    ["Width"]=>
    int(1)
    ["IsColor"]=>
    int(1)
  }
}
--CREDITS--
Eric Stewart <ericleestewart@gmail.com>
