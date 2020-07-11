--TEST--
Bug #50845 (exif_read_data() returns corrupted exif headers)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
$infile = __DIR__.'/bug50845.jpg';
var_dump(exif_read_data($infile));
--EXPECTF--
array(44) {
  ["FileName"]=>
  string(12) "bug50845.jpg"
  ["FileDateTime"]=>
  int(%d)
  ["FileSize"]=>
  int(803603)
  ["FileType"]=>
  int(2)
  ["MimeType"]=>
  string(10) "image/jpeg"
  ["SectionsFound"]=>
  string(30) "ANY_TAG, IFD0, THUMBNAIL, EXIF"
  ["COMPUTED"]=>
  array(9) {
    ["html"]=>
    string(26) "width="5472" height="3648""
    ["Height"]=>
    int(3648)
    ["Width"]=>
    int(5472)
    ["IsColor"]=>
    int(1)
    ["ByteOrderMotorola"]=>
    int(0)
    ["ApertureFNumber"]=>
    string(5) "f/7.1"
    ["Copyright"]=>
    string(13) "Public Domain"
    ["Thumbnail.FileType"]=>
    int(2)
    ["Thumbnail.MimeType"]=>
    string(10) "image/jpeg"
  }
  ["ImageDescription"]=>
  string(295) "A U.S. Marine Corps MV-22 Osprey lands on the USS Whidbey Island (LSD-41), May 5, 2016. The vehicles were loaded to support a theater security cooperation event as a part of a MEU readiness exercise. (U.S. Marine Corps photo by Lance Cpl. Koby I. Saunders/22 Marine Expeditionary Unit/ Released)"
  ["Make"]=>
  string(5) "Canon"
  ["Model"]=>
  string(22) "Canon EOS-1D X Mark II"
  ["Orientation"]=>
  int(1)
  ["XResolution"]=>
  string(5) "240/1"
  ["YResolution"]=>
  string(5) "240/1"
  ["ResolutionUnit"]=>
  int(2)
  ["Artist"]=>
  string(24) "Lance Cpl. Koby Saunders"
  ["Copyright"]=>
  string(13) "Public Domain"
  ["Exif_IFD_Pointer"]=>
  int(12572)
  ["THUMBNAIL"]=>
  array(6) {
    ["Compression"]=>
    int(6)
    ["XResolution"]=>
    string(5) "240/1"
    ["YResolution"]=>
    string(5) "240/1"
    ["ResolutionUnit"]=>
    int(2)
    ["JPEGInterchangeFormat"]=>
    int(860)
    ["JPEGInterchangeFormatLength"]=>
    int(11204)
  }
  ["ExposureTime"]=>
  string(5) "1/200"
  ["FNumber"]=>
  string(5) "71/10"
  ["ExposureProgram"]=>
  int(1)
  ["ISOSpeedRatings"]=>
  int(100)
  ["UndefinedTag:0x8830"]=>
  int(2)
  ["UndefinedTag:0x8832"]=>
  int(100)
  ["ExifVersion"]=>
  string(4) "0230"
  ["ShutterSpeedValue"]=>
  string(15) "7643856/1000000"
  ["ApertureValue"]=>
  string(15) "5655638/1000000"
  ["ExposureBiasValue"]=>
  string(3) "0/1"
  ["MaxApertureValue"]=>
  string(3) "4/1"
  ["MeteringMode"]=>
  int(5)
  ["Flash"]=>
  int(16)
  ["FocalLength"]=>
  string(4) "24/1"
  ["ColorSpace"]=>
  int(65535)
  ["FocalPlaneXResolution"]=>
  string(12) "5472000/1438"
  ["FocalPlaneYResolution"]=>
  string(11) "3648000/958"
  ["FocalPlaneResolutionUnit"]=>
  int(2)
  ["CustomRendered"]=>
  int(0)
  ["ExposureMode"]=>
  int(1)
  ["WhiteBalance"]=>
  int(0)
  ["SceneCaptureType"]=>
  int(0)
  ["UndefinedTag:0xA431"]=>
  string(12) "002099000358"
  ["UndefinedTag:0xA432"]=>
  array(4) {
    [0]=>
    string(4) "24/1"
    [1]=>
    string(5) "105/1"
    [2]=>
    string(3) "0/0"
    [3]=>
    string(3) "0/0"
  }
  ["UndefinedTag:0xA434"]=>
  string(22) "EF24-105mm f/4L IS USM"
  ["UndefinedTag:0xA435"]=>
  string(10) "000044bc4c"
}
