--TEST--
Bug #72627 (Memory Leakage In exif_process_IFD_in_TIFF)
--EXTENSIONS--
exif
--FILE--
<?php
    $exif = exif_read_data(__DIR__ . '/bug72627.tiff',0,0,true);
    var_dump($exif);
?>
--EXPECTF--
Warning: exif_read_data(%s): Thumbnail goes IFD boundary or end of file reached in %sbug72627.php on line %d

Warning: exif_read_data(%s): Error in TIFF: filesize(x04E2) less than start of IFD dir(x829A0004) in %sbug72627.php on line %d

Warning: exif_read_data(%s): Thumbnail goes IFD boundary or end of file reached in %sbug72627.php on line %d
array(11) {
  ["FileName"]=>
  string(13) "bug72627.tiff"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(1250)
  ["FileType"]=>
  int(7)
  ["MimeType"]=>
  string(10) "image/tiff"
  ["SectionsFound"]=>
  string(30) "ANY_TAG, IFD0, THUMBNAIL, EXIF"
  ["COMPUTED"]=>
  array(10) {
    ["html"]=>
    string(24) "width="128" height="132""
    ["Height"]=>
    int(132)
    ["Width"]=>
    int(128)
    ["IsColor"]=>
    int(0)
    ["ByteOrderMotorola"]=>
    int(0)
    ["ApertureFNumber"]=>
    string(5) "f/1.0"
    ["Thumbnail.FileType"]=>
    int(2)
    ["Thumbnail.MimeType"]=>
    string(10) "image/jpeg"
    ["Thumbnail.Height"]=>
    int(132)
    ["Thumbnail.Width"]=>
    int(128)
  }
  ["XResolution"]=>
  string(21) "1414812756/1414812756"
  ["THUMBNAIL"]=>
  array(5) {
    ["ImageWidth"]=>
    int(128)
    ["ImageLength"]=>
    int(132)
    ["JPEGInterchangeFormat"]=>
    int(1280)
    ["JPEGInterchangeFormatLength"]=>
    int(100)
    ["THUMBNAIL"]=>
    NULL
  }
  ["ExposureTime"]=>
  string(21) "1414812756/1414812756"
  ["FNumber"]=>
  string(21) "1414812756/1414812756"
}
