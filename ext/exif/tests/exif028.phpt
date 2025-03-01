--TEST--
Check for exif_read_data, HEIF with IFD0 and EXIF data in Motorola byte-order.
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(__DIR__.'/../../standard/tests/image/test4pix.heic'));
?>
--EXPECTF--
array(53) {
  ["FileName"]=>
  string(13) "test4pix.heic"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(42199)
  ["FileType"]=>
  int(20)
  ["MimeType"]=>
  string(10) "image/heif"
  ["SectionsFound"]=>
  string(19) "ANY_TAG, IFD0, EXIF"
  ["COMPUTED"]=>
  array(3) {
    ["IsColor"]=>
    int(0)
    ["ByteOrderMotorola"]=>
    int(1)
    ["ApertureFNumber"]=>
    string(5) "f/1.8"
  }
  ["Make"]=>
  string(5) "Apple"
  ["Model"]=>
  string(26) "iPhone SE (3rd generation)"
  ["Orientation"]=>
  int(1)
  ["XResolution"]=>
  string(4) "72/1"
  ["YResolution"]=>
  string(4) "72/1"
  ["ResolutionUnit"]=>
  int(2)
  ["Software"]=>
  string(6) "17.2.1"
  ["DateTime"]=>
  string(19) "2024:02:21 16:03:50"
  ["HostComputer"]=>
  string(26) "iPhone SE (3rd generation)"
  ["TileWidth"]=>
  int(512)
  ["TileLength"]=>
  int(512)
  ["Exif_IFD_Pointer"]=>
  int(264)
  ["ExposureTime"]=>
  string(4) "1/60"
  ["FNumber"]=>
  string(3) "9/5"
  ["ExposureProgram"]=>
  int(2)
  ["ISOSpeedRatings"]=>
  int(200)
  ["ExifVersion"]=>
  string(4) "0232"
  ["DateTimeOriginal"]=>
  string(19) "2024:02:21 16:03:50"
  ["DateTimeDigitized"]=>
  string(19) "2024:02:21 16:03:50"
  ["UndefinedTag:0x9010"]=>
  string(6) "+08:00"
  ["UndefinedTag:0x9011"]=>
  string(6) "+08:00"
  ["UndefinedTag:0x9012"]=>
  string(6) "+08:00"
  ["ShutterSpeedValue"]=>
  string(12) "159921/27040"
  ["ApertureValue"]=>
  string(11) "54823/32325"
  ["BrightnessValue"]=>
  string(11) "29968/13467"
  ["ExposureBiasValue"]=>
  string(3) "0/1"
  ["MeteringMode"]=>
  int(5)
  ["Flash"]=>
  int(16)
  ["FocalLength"]=>
  string(7) "399/100"
  ["SubjectLocation"]=>
  array(4) {
    [0]=>
    int(1995)
    [1]=>
    int(1507)
    [2]=>
    int(2217)
    [3]=>
    int(1332)
  }
  ["MakerNote"]=>
  string(9) "Apple iOS"
  ["SubSecTimeOriginal"]=>
  string(3) "598"
  ["SubSecTimeDigitized"]=>
  string(3) "598"
  ["ColorSpace"]=>
  int(65535)
  ["ExifImageWidth"]=>
  int(4032)
  ["ExifImageLength"]=>
  int(3024)
  ["SensingMethod"]=>
  int(2)
  ["SceneType"]=>
  string(1) ""
  ["ExposureMode"]=>
  int(0)
  ["WhiteBalance"]=>
  int(0)
  ["DigitalZoomRatio"]=>
  string(7) "756/151"
  ["FocalLengthIn35mmFilm"]=>
  int(140)
  ["UndefinedTag:0xA432"]=>
  array(4) {
    [0]=>
    string(15) "4183519/1048501"
    [1]=>
    string(15) "4183519/1048501"
    [2]=>
    string(3) "9/5"
    [3]=>
    string(3) "9/5"
  }
  ["UndefinedTag:0xA433"]=>
  string(5) "Apple"
  ["UndefinedTag:0xA434"]=>
  string(51) "iPhone SE (3rd generation) back camera 3.99mm f/1.8"
  ["UndefinedTag:0xA460"]=>
  int(2)
}
