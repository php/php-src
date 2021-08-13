--TEST--
Check for exif_read_data, JPEG with IFD, EXIF and GPS data in Motorola byte-order.
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(__DIR__.'/image015.jpg'));
?>
--EXPECTF--
array(37) {
  ["FileName"]=>
  string(12) "image015.jpg"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(%d)
  ["FileType"]=>
  int(2)
  ["MimeType"]=>
  string(10) "image/jpeg"
  ["SectionsFound"]=>
  string(24) "ANY_TAG, IFD0, EXIF, GPS"
  ["COMPUTED"]=>
  array(9) {
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
    ["ApertureFNumber"]=>
    string(5) "f/8.0"
    ["Copyright"]=>
    string(24) "Eric Stewart, Hex Editor"
    ["Copyright.Photographer"]=>
    string(12) "Eric Stewart"
    ["Copyright.Editor"]=>
    string(10) "Hex Editor"
  }
  ["ImageDescription"]=>
  string(15) "My description."
  ["Make"]=>
  string(11) "OpenShutter"
  ["Model"]=>
  string(8) "OS 1.0.0"
  ["XResolution"]=>
  string(4) "72/1"
  ["YResolution"]=>
  string(4) "72/1"
  ["ResolutionUnit"]=>
  int(2)
  ["DateTime"]=>
  string(19) "2008:06:19 01:47:53"
  ["Artist"]=>
  string(12) "Eric Stewart"
  ["Copyright"]=>
  string(12) "Eric Stewart"
  ["Exif_IFD_Pointer"]=>
  int(258)
  ["GPS_IFD_Pointer"]=>
  int(460)
  ["ExposureTime"]=>
  string(5) "1/125"
  ["FNumber"]=>
  string(3) "8/1"
  ["ISOSpeedRatings"]=>
  int(80)
  ["DateTimeOriginal"]=>
  string(19) "2008:06:19 01:47:53"
  ["DateTimeDigitized"]=>
  string(19) "2008:06:19 01:47:53"
  ["MeteringMode"]=>
  int(5)
  ["LightSource"]=>
  int(4)
  ["Flash"]=>
  int(7)
  ["FocalLength"]=>
  string(4) "29/5"
  ["ExifImageWidth"]=>
  int(1)
  ["ExifImageLength"]=>
  int(1)
  ["GPSVersion"]=>
  string(4) "%0%0"
  ["GPSLatitudeRef"]=>
  string(1) "N"
  ["GPSLatitude"]=>
  array(3) {
    [0]=>
    string(4) "33/1"
    [1]=>
    string(4) "37/1"
    [2]=>
    string(3) "0/1"
  }
  ["GPSLongitudeRef"]=>
  string(1) "W"
  ["GPSLongitude"]=>
  array(3) {
    [0]=>
    string(4) "84/1"
    [1]=>
    string(3) "7/1"
    [2]=>
    string(3) "0/1"
  }
  ["GPSAltitudeRef"]=>
  string(1) "%0"
  ["GPSAltitude"]=>
  string(5) "295/1"
  ["GPSTimeStamp"]=>
  array(3) {
    [0]=>
    string(3) "1/1"
    [1]=>
    string(4) "47/1"
    [2]=>
    string(4) "53/1"
  }
}
--CREDITS--
Eric Stewart <ericleestewart@gmail.com>
