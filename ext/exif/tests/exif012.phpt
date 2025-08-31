--TEST--
Check for exif_read_data, JPEG with IFD and GPS data in Intel byte-order.
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
var_dump(exif_read_data(__DIR__.'/image012.jpg'));
?>
--EXPECTF--
array(25) {
  ["FileName"]=>
  string(12) "image012.jpg"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(%d)
  ["FileType"]=>
  int(2)
  ["MimeType"]=>
  string(10) "image/jpeg"
  ["SectionsFound"]=>
  string(18) "ANY_TAG, IFD0, GPS"
  ["COMPUTED"]=>
  array(8) {
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
  ["GPS_IFD_Pointer"]=>
  int(246)
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
