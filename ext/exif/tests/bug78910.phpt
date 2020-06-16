--TEST--
Bug #78910: Heap-buffer-overflow READ in exif (OSS-Fuzz #19044)
--FILE--
<?php

var_dump(exif_read_data('data:image/jpg;base64,TU0AKgAAAAwgICAgAAIBDwAEAAAAAgAAACKSfCAgAAAAAEZVSklGSUxN'));

?>
--EXPECTF--
Notice: exif_read_data(): Read from TIFF: tag(0x927C,   MakerNote): Illegal format code 0x2020, switching to BYTE in %s on line %d

Warning: exif_read_data(): Process tag(x927C=MakerNote): Illegal format code 0x2020, suppose BYTE in %s on line %d

Warning: exif_read_data(): IFD data too short: 0x0000 offset 0x000C in %s on line %d

Warning: exif_read_data(): Error in TIFF: filesize(x002A) less than start of IFD dir(x46494C4F) in %s on line %d
array(8) {
  ["FileDateTime"]=>
  int(0)
  ["FileSize"]=>
  int(42)
  ["FileType"]=>
  int(8)
  ["MimeType"]=>
  string(10) "image/tiff"
  ["SectionsFound"]=>
  string(13) "ANY_TAG, IFD0"
  ["COMPUTED"]=>
  array(2) {
    ["IsColor"]=>
    int(0)
    ["ByteOrderMotorola"]=>
    int(1)
  }
  ["Make"]=>
  array(2) {
    [0]=>
    int(1179994697)
    [1]=>
    int(1179208781)
  }
  ["MakerNote"]=>
  NULL
}
