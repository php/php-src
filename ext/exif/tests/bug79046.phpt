--TEST--
Bug #79046: NaN to int cast undefined behavior in exif
--FILE--
<?php
var_dump(exif_read_data('data://image/tiff;base64,TU0AKgAAAA7//wAAANUAAQERAAsAAAABAAD4fwAAAA4A'));
?>
--EXPECT--
array(8) {
  ["FileDateTime"]=>
  int(0)
  ["FileSize"]=>
  int(33)
  ["FileType"]=>
  int(8)
  ["MimeType"]=>
  string(10) "image/tiff"
  ["SectionsFound"]=>
  string(24) "ANY_TAG, IFD0, THUMBNAIL"
  ["COMPUTED"]=>
  array(2) {
    ["IsColor"]=>
    int(0)
    ["ByteOrderMotorola"]=>
    int(1)
  }
  ["StripOffsets"]=>
  float(NAN)
  ["THUMBNAIL"]=>
  array(1) {
    ["StripOffsets"]=>
    float(NAN)
  }
}
